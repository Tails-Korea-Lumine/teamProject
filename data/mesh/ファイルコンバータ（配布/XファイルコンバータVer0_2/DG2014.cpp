#include "DG2014.h"
#include "MyPG.h"

//-------------------------------------------------------------
#define kFileHeader "3DMesh YS Ver0.2"

struct sFileHeader
{
	char  Kind[32];
};
//マテリアルコンテナ
class cMaterialContainerS{
public:
	char					name[64];	//マテリアル名
	ML::Color				diffuse;	//マテリアル情報:デフューズ
	ML::Color				ambient;	//マテリアル情報:アンビエント
	ML::Color				speculer;	//マテリアル情報:スペキュラ
	ML::Color				emissive;	//マテリアル情報:エミッシブ
	char					texName[64];//マテリアル名
	char					bw[320];//未使用エリア（合計512バイト）
};
class cMeshGroupS{
public:
	DWORD					mcNumber;	//対応するマテリアル番号
	DWORD					indexNum;	//インデックス数
	vector<DWORD>			vIndex;		//インデックス
};

vector<cMaterialContainerS>	mateConts;
vector<cMeshGroupS> meshGroups;


struct sVertex{	D3DXVECTOR3  v, n;	D3DXVECTOR2 t;};

namespace DG
{
	bool  SaveFileName(tstring&  fpath_)
	{
		//	ファイルを保存する準備をします
		HANDLE			file			= NULL;
		OPENFILENAME	openFileName	= { ( DWORD )0 };
		char			filePath[256]	= "";
		strcpy(filePath,  fpath_.c_str( ));
		char			fileTitle[64]	= "";
		//　構造体の設定をします
		ZeroMemory( &openFileName, sizeof( openFileName ) );
		openFileName.lStructSize		= sizeof( openFileName );
		openFileName.lpstrTitle			= "保存するSOBファイル名を指定して下さい";
		openFileName.hwndOwner			= ge->wh;
		openFileName.lpstrFilter		= "SOBﾌｧｲﾙ(*.SOB)\0\0";
		openFileName.nFilterIndex		= 1;
		openFileName.lpstrFile			= filePath;
		openFileName.nMaxFile			= sizeof( filePath );
		openFileName.lpstrFileTitle		= fileTitle;
		openFileName.nMaxFileTitle		= sizeof( fileTitle );
		openFileName.lpstrDefExt		= "SOB";
		openFileName.Flags				=
			OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST |
			OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;// | OFN_ALLOWMULTISELECT;
		//	保存するファイルのパスを取得
		if( !GetSaveFileName( &openFileName ) )
		{
			return false;
		}
		fpath_ = filePath;
		return true;
	}

	bool  cMesh::W_SOB(const tstring&	fpath_)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	return  false; }
		//メッシュから頂点情報を頂く
		IDirect3DVertexBuffer9*   vb;
		this->mesh->GetVertexBuffer(&vb);
		//頂点フォーマットから、１頂点当たりのデータサイズを取得する
		D3DVERTEXBUFFER_DESC vbDesc;
		vb->GetDesc(&vbDesc);
		DWORD  vertexSize  =  D3DXGetFVFVertexSize( vbDesc.FVF );
		//頂点バッファのサイズから、頂点数を求める
		DWORD  numVertex  =  vbDesc.Size  /  vertexSize;
		//頂点情報に、頂点座標・法線・UVが含まれるか調べる
		bool  bPos,  bNormal,  bUV;
		bPos	=  (vbDesc.FVF  &  D3DFVF_XYZ) ? true : false;
		bNormal	=  (vbDesc.FVF  &  D3DFVF_NORMAL) ? true : false;
		bUV		=  (vbDesc.FVF  &  D3DFVF_TEX1) ? true : false;
		if(!bPos  ||  !bNormal  ||  !bUV){
			return false;
		}
		//各情報が、先頭から何バイト目にあるか調べる
		DWORD  posPos		=  D3DXGetFVFVertexSize( (vbDesc.FVF % D3DFVF_XYZ) );
		DWORD  normalPos	=  D3DXGetFVFVertexSize( (vbDesc.FVF % D3DFVF_NORMAL) );
		DWORD  uvPos		=  D3DXGetFVFVertexSize( (vbDesc.FVF % D3DFVF_TEX1) );
		//全頂点を受け取れるバッファを準備
		vector<ML::Vec3>  vCoord;		vCoord.reserve(numVertex);
		vector<ML::Vec3>  vNormal;		vNormal.reserve(numVertex);
		vector<ML::Vec2>  vUV;			vUV.reserve(numVertex);
		
		//バーテックスバッファからデータをちゅるちゅるする
		void  *pVB_void;
		vb->Lock(0, 0, &pVB_void, D3DLOCK_READONLY);
		BYTE  *pVB = (BYTE*)pVB_void;

		for(DWORD  c=0;  c < numVertex; c++,  pVB += vertexSize){
			D3DXVECTOR3  *p, *n;
			D3DXVECTOR2  *uv;
			p  = (D3DXVECTOR3*)(pVB + posPos);
			n  = (D3DXVECTOR3*)(pVB + normalPos);
			uv = (D3DXVECTOR2*)(pVB + uvPos);
			vCoord.push_back(*p);
			vNormal.push_back(*n);
			vUV.push_back(*uv);
		}
		vb->Unlock( );
		//インデックスバッファにアクセス
		LPDIRECT3DINDEXBUFFER9  wib;
		this->mesh->GetIndexBuffer(&wib);
		D3DINDEXBUFFER_DESC  ibDesc;
		wib->GetDesc(&ibDesc);

		DWORD  indexSize;
		if(ibDesc.Format == D3DFMT_INDEX16){		indexSize = 2;	}
		else if(ibDesc.Format == D3DFMT_INDEX32){	indexSize = 4;	}
		else{	return  false;}

		DWORD  numIndex = 0;
		numIndex = ibDesc.Size / indexSize;
		//インデックスを全部納められるバッファを作る
		vector<DWORD>  index;		index.reserve(numIndex);
		//インデックスをメッシュからちゅるちゅる
		void  *pIB_void;
		wib->Lock(0, 0, &pIB_void, D3DLOCK_READONLY);
		BYTE  *pIB = (BYTE*)pIB_void;

		for(DWORD  c=0;  c < numIndex; c++,  pIB += indexSize){
			if(indexSize == 2){
				WORD  *i;
				i = (WORD*)pIB;
				index.push_back( (DWORD)(*i) );
			}else if(indexSize == 4){
				DWORD  *i;
				i = (DWORD*)pIB;
				index.push_back( *i );
			}
		}
		wib->Unlock( );
		//属性情報にアクセス
		DWORD numAT;
		mesh->GetAttributeTable(nullptr,  &numAT);
		vector<D3DXATTRIBUTERANGE>  attribute;
		attribute.resize(numAT);
		mesh->GetAttributeTable(&attribute.front( ),  &numAT);
		//
		vector<cMeshGroupS>  ms;
		ms.resize(numAT);
		for(DWORD  c=0;  c < numAT; c++){
			ms[c].mcNumber  =  attribute[c].AttribId;
			ms[c].indexNum  =  attribute[c].FaceCount*3;
			ms[c].vIndex.resize(attribute[c].FaceCount*3);
			for(DWORD i=0; i < ms[c].indexNum; i++){
				ms[c].vIndex[i]  =  index[attribute[c].FaceStart + i];
			}
		}
		////////////////
		vector<cMaterialContainerS>  mg;
		if(materials != NULL){
		//	マテリアル情報バッファへのポインタ取得
			D3DXMATERIAL* material = (D3DXMATERIAL*)materials->GetBufferPointer( );
			DWORD numMaterials  =  materials->GetBufferSize( )  /  sizeof(D3DXMATERIAL);
			mg.resize(numMaterials);

			for(DWORD  c=0;  c<numMaterials;  c++){
				mg[c].ambient  =  ML::Color(
					material[c].MatD3D.Ambient.a, material[c].MatD3D.Ambient.r,
					material[c].MatD3D.Ambient.g, material[c].MatD3D.Ambient.b);
				mg[c].diffuse  =  ML::Color(
					material[c].MatD3D.Diffuse.a, material[c].MatD3D.Diffuse.r,
					material[c].MatD3D.Diffuse.g, material[c].MatD3D.Diffuse.b);
				mg[c].speculer  =  ML::Color(
					material[c].MatD3D.Specular.a, material[c].MatD3D.Specular.r,
					material[c].MatD3D.Specular.g, material[c].MatD3D.Specular.b);
				mg[c].emissive  =  ML::Color(
					material[c].MatD3D.Emissive.a, material[c].MatD3D.Emissive.r,
					material[c].MatD3D.Emissive.g, material[c].MatD3D.Emissive.b);

				if(material[c].pTextureFilename){
					if(strlen(material[c].pTextureFilename) < sizeof(mg[c].texName)){
						strcpy_s(mg[c].texName, sizeof(mg[c].texName), material[c].pTextureFilename);
					}
				}
			}
		}
		//多分、準備はこれで終了

		//ファイルに出力しますよ～
		FILE *fpW;
		//
		fopen_s(&fpW, fpath_.c_str( ),"wb");

		//ファイルヘッダ
		sFileHeader  kind = {kFileHeader};
		fwrite(&kind, sizeof(kind), 1, fpW);

		//マテリアルコンテナ書きますよ
		DWORD  mc = mg.size( );
		fwrite(&mc, sizeof(mc), 1, fpW);
		fwrite(&mg[0], sizeof(cMaterialContainerS), mg.size( ), fpW);

		//メッシュグループ書きますよ
		DWORD  mgs = ms.size( );
		fwrite(&mgs, sizeof(mgs), 1, fpW);//件数
		for(DWORD  c = 0; c < ms.size( ); c++){
			fwrite(&ms[c].mcNumber, sizeof(DWORD), 1, fpW);//対応マテリアル番号
			fwrite(&ms[c].indexNum, sizeof(DWORD), 1, fpW);//インデックス数
			DWORD tm =  ms[c].vIndex.size( );
			fwrite(&ms[c].vIndex[0], sizeof(DWORD), ms[c].vIndex.size( ), fpW);//インデックス配列
		}

		//締めはバーテックスですよ
		char  vcTitle[16] = "POSITION";
		fwrite(vcTitle, sizeof(vcTitle), 1, fpW);//ヘッダ名
		DWORD  vc = vCoord.size( );
		fwrite(&vc, sizeof(vc), 1, fpW);//件数
		fwrite(&vCoord[0], sizeof(ML::Vec3), vCoord.size( ), fpW);//データ


		char  ncTitle[16] = "NORMAL";
		fwrite(ncTitle, sizeof(ncTitle), 1, fpW);//ヘッダ名
		DWORD  nc = vNormal.size( );
		fwrite(&nc, sizeof(nc), 1, fpW);//件数
		fwrite(&vNormal[0], sizeof(ML::Vec3), vNormal.size( ), fpW);//データ


		char  tcTitle[16] = "TEX_UV";
		fwrite(tcTitle, sizeof(tcTitle), 1, fpW);//ヘッダ名
		DWORD  tc = vUV.size( );
		fwrite(&tc, sizeof(tc), 1, fpW);//件数
		fwrite(&vUV[0], sizeof(ML::Vec2), vUV.size( ), fpW);//データ


		fclose(fpW);	
		return true;
	}

}
//-------------------------------------------------------------







//┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//┃画像制御系機能群																		　┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
namespace DG
{
	cObj::WP cObj::winst;	//	１つしか存在できないインスタンス

	cObj::cObj(		HWND hw_,
					int w_,
					int h_,
					bool sm_) :
		d3dDevice( ),
		d3d( ),
		caps( ),
		sprite( )
	{
		D3DPRESENT_PARAMETERS pp;
	//	Direct3Dオブジェクトの生成
		d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if(d3d == 0){	throw ERR_CREATE;	}
	//	デバイスの能力を確認
		ZeroMemory(&caps, sizeof(caps));
		d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	//	デバイスの設定
		ZeroMemory(&pp, sizeof(pp));
		//	解像度セット
		pp.BackBufferWidth = w_;
		pp.BackBufferHeight = h_;
		if(sm_ == true){
		// ウインドウモードではない
			pp.Windowed = FALSE;
		}
		else{
		//	ウインドウモード
			pp.Windowed = TRUE;
		}

	//	バックバッファ、レンダリング情報
		pp.BackBufferFormat = D3DFMT_X8R8G8B8;
		pp.BackBufferCount = 2;
		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_FLIP;
	//	ステンシルバッファを有効に（フォーマットは16ビットフォーマット）
		pp.EnableAutoDepthStencil = TRUE;
		pp.AutoDepthStencilFormat = D3DFMT_D16;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

		//	ハードウェア機能選択用の情報
		struct{	_D3DDEVTYPE dt; DWORD vp; }modeTable[3] = {
			{D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING},	//	T&L HAL
			{D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING},	//	HAL
			{D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING}	//	REF
		};
	//	デバイスの作成
		int cnt;
		BOOL devFlag = TRUE;
		for(cnt = 0; cnt < 3 && devFlag == TRUE; cnt++){
			devFlag = FAILED(
				d3d->CreateDevice(	D3DADAPTER_DEFAULT,
									modeTable[cnt].dt,
									hw_,
									modeTable[cnt].vp,
									&pp,
									&d3dDevice)
			);
		}
	//	デバイス生成失敗
		if(devFlag == TRUE){	d3d->Release( ); throw ERR_CREATE;}
	//	レンダリング時の法線正規化ON
		d3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	//	スプライトレンダリングオブジェクトの生成
		if(FAILED(D3DXCreateSprite(d3dDevice, &sprite))){	throw ERR_CREATE;}
	}
	cObj::~cObj( )
	{
	//	スプライトの解放
		ML::SafeRelease(&sprite);
	//	D3Dデバイスの解放
		ML::SafeRelease(&d3dDevice);
	//	D3Dオブジェクトの解放
		ML::SafeRelease(&d3d);
	}
	//	DGライブラリを生成する
	cObj::SP cObj::Create(	HWND		hw_,	//	ウィンドウハンドル
							int			w_,		//	横幅
							int			h_,		//	縦幅
							bool		sm_)	//	スクリーンモード
	{
		if(winst.expired( )){
			cObj::SP sp = cObj::SP(new cObj(hw_, w_, h_, sm_));
			winst = sp;
			return sp;
		}
		return 0;
	}
	cObj::SP cObj::GetInst( )
	{
		if(!winst.expired( )){
			return winst.lock( );
		}
		return 0;
	}
//	画面描画（開始）
	BOOL cObj::Begin(	DWORD color_)
	{
	//	バックバッファをクリア（画面消去
		if( FAILED( d3dDevice->Clear(	0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
									color_, 1.0f, 0) ) ){return false;}
	//	描画開始許可を取る
		if( FAILED( d3dDevice->BeginScene( ) ) ){return false;}
		return true;
	}
//	3D描画の許可を得る
	void cObj::Begin3D( )
	{
	}
//	3D描画を終了する
	void  cObj::End3D( )
	{
	}
//	2D描画の許可を得る
	void  cObj::Begin2D( )
	{
		sprite->Begin(D3DXSPRITE_ALPHABLEND);
	}
//	2D描画のを終了する
	void  cObj::End2D( )
	{
		sprite->End( );
	}
//	画面描画（終了）
	void  cObj::End( )
	{
	//	描画終了
		d3dDevice->EndScene( );
	//	バックバッファをプライマリバッファにコピー or フリップ
		if (FAILED(d3dDevice->Present(NULL, NULL, NULL, NULL))){
			d3dDevice->Reset(NULL);
		}
	}
	//	デバイス
	LPDIRECT3DDEVICE9 cObj::Device( ){	return d3dDevice;	}
	LPD3DXSPRITE cObj::Sprite( ){		return sprite;	}
	//------------------------------------------------------------
	cMesh::cMesh(	const tstring&		fpath_,
					const D3DXMATRIX&	matDef_)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	throw ERR_NOTDEVICE; }

	//	───────────────────────────────────
	//	Ｘファイルからメッシュとマテリアル情報を取得
		if(FAILED(	D3DXLoadMeshFromX(	fpath_.c_str( ),
										D3DXMESH_MANAGED,
										dgi->Device( ),
										NULL,
										&materials,
										NULL,
										&numMaterials,
										&mesh))){
			MessageBox(NULL, fpath_.c_str( ), _T("メッシュの読み込みに失敗しました"), MB_OK);
			throw ERR_CREATE;
		}
	//	───────────────────────────────────
	//	読み込んだXファイルのフォーマットを得る
		DWORD fvf = mesh->GetFVF();

	//	───────────────────────────────────
	//	マテリアル情報に対する処理
		tstring path = ML::GetPath(fpath_);
		if(materials != NULL){
		//	マテリアル情報バッファへのポインタ取得
			D3DXMATERIAL* material = (D3DXMATERIAL*)materials->GetBufferPointer( );
		//	テクスチャ用の領域を作成
			textures = new LPDIRECT3DTEXTURE9[numMaterials];
		//	マテリアル情報から「色情報」と「テクスチャファイル名」を取得
			for(DWORD c = 0; c < numMaterials; c++){
				D3DMATERIAL9* mat = &material[c].MatD3D;
			//	マテリアル情報にアンビエント値がない場合、強制的に設定する
				if(	mat->Ambient.r == 0.0f && mat->Ambient.g == 0.0f &&
					mat->Ambient.b == 0.0f ){
						mat->Ambient.a = 1.0f;	mat->Ambient.r = 1.0f;
						mat->Ambient.g = 1.0f;	mat->Ambient.b = 1.0f;
					}
			//	テクスチャファイル名がある場合、テクスチャを読み込む
				if(material[c].pTextureFilename != NULL){
					tstringstream ss;
					ss << path << _T("/") << material[c].pTextureFilename;
					tstring tfp;
					ss >> tfp;

					if(FAILED( D3DXCreateTextureFromFile( 
						dgi->Device( ), tfp.c_str( ), &textures[c]) ) ){
						MessageBox(NULL, tfp.c_str( ), _T("テクスチャの読み込みに失敗しました"), MB_OK);
						textures[c] = NULL;
					}
				}
			}
		//	───────────────────────────────────
		//	Xファイルのメッシュ情報を、本プログラム用に変換
		//	クローンの設定
			D3DVERTEXELEMENT9 decl[] = 
			{
				{	0,	0,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0},
				{	0, 12,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0},	
				{	0, 24,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0},
				D3DDECL_END( )
			//	0         1           2       3
			//	01234567890123456789012345678901
			//  X---Y---Z---X---Y---Z---U---V---
			//  頂点座標----法線Vec-----tex座標
			//	つまり、頂点座標、法線ベクトル、テクスチャ座標を持ったクローンを生成する為の設定
			};
		//	クローンを生成する
			LPD3DXMESH pOld = mesh;
			if(FAILED(pOld->CloneMesh(D3DXMESH_MANAGED, decl, dgi->Device( ), &mesh))){
				throw ERR_CREATE;
			}
//////////////////////////////////////////////////////////
			tstring  svfp = fpath_;
			if(SaveFileName(svfp)){
				W_SOB(svfp);
			}
//////////////////////////////////////////////////////////

		//	クローンを残し、オリジナルを解放する
			pOld->Release( );

		//	Xファイルから読み込んだメッシュに法線情報が無い場合
			if(!(fvf & D3DFVF_NORMAL)){	D3DXComputeNormals(mesh, NULL);}
		//	デフォルト変換行列の設定
			D3DXMATRIX md;
			D3DXMatrixIdentity(&md);
			matDef = matDef_;
		}

		//メッシュからバウンディングボックスを生成する
		///バーテックスバッファのロック
		void*  vtBP;
		mesh->LockVertexBuffer(D3DLOCK_READONLY,  &vtBP);
		///頂点数の取得
		DWORD  vtNum  =  mesh->GetNumVertices( );
		//頂点単位のバイト数の取得
		DWORD  vtSize =  D3DXGetFVFVertexSize( mesh->GetFVF( ) );

		///バウンディングボックスの生成
		D3DXComputeBoundingBox((LPD3DXVECTOR3)vtBP,  vtNum,  vtSize,  &bBox.f,  &bBox.r);

		///バーテックスバッファのアンロック
		mesh->UnlockVertexBuffer( );
		vtBP = nullptr;
	}
	cMesh::~cMesh( )
	{
	//	テクスチャテーブルを開放する
		if(textures != NULL){
		//	個々のテクスチャを開放する
			for(DWORD c = 0; c < numMaterials; c++){
				ML::SafeRelease(&textures[c]);
			}
		//	テーブルを開放
			delete[] textures;
			textures = NULL;
		}
	//	マテリアルを開放する
		ML::SafeRelease(&materials);
	//	メッシュを開放する
		ML::SafeRelease(&mesh);
	}
	//	メッシュを生成する
	cMesh::SP cMesh::Create(	const tstring&		fpath_,
								const D3DXMATRIX&	matDef_)
	{
		return SP( new cMesh(fpath_, matDef_) );
	}
	//	メッシュを表示する
	void cMesh::Draw(	const ML::Vec3&	pos_,		//	表示座標
						float				rotY_)		//	回転角度（ラジアン)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	throw ERR_NOTDEVICE; }
		D3DXMATRIX matW, matRY, matT;
	//	Y軸に対する回転行列を作成する
		D3DXMatrixRotationY(&matRY, rotY_);
	//	平行移動行列を作成する
		D3DXMatrixTranslation(&matT, pos_.x, pos_.y, pos_.z);
	//	行列の合成
		matW = matRY * matT;
	//	ワールド変換行列を設定する
		dgi->Device( )->SetTransform(D3DTS_WORLD, &matW);
		
	//	メッシュをマテリアル毎に描画
		D3DXMATERIAL* material = (D3DXMATERIAL*)materials->GetBufferPointer( );
		for(DWORD c = 0; c < numMaterials; c++){
			dgi->Device( )->SetMaterial(&material[c].MatD3D);	//	レンダリングに使用するマテリアルを設定
			if(textures){
				dgi->Device( )->SetTexture(0, textures[c]);		//	レンダリングに使用するテクスチャを設定
			}
			else{
				dgi->Device( )->SetTexture(0, nullptr);		//	レンダリングに使用するテクスチャを設定
			}
			mesh->DrawSubset(c);								//	メッシュをレンダリングする
		}
	}
	//	メッシュを表示する
	void cMesh::Draw(	const D3DXMATRIX&	mat_)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	throw ERR_NOTDEVICE; }
	//	ワールド変換行列を設定する
		dgi->Device( )->SetTransform(D3DTS_WORLD, &mat_);
		
	//	メッシュをマテリアル毎に描画
		D3DXMATERIAL* material = (D3DXMATERIAL*)materials->GetBufferPointer( );
		for(DWORD c = 0; c < numMaterials; c++){
			dgi->Device( )->SetMaterial(&material[c].MatD3D);	//	レンダリングに使用するマテリアルを設定
			dgi->Device( )->SetTexture(0, textures[c]);		//	レンダリングに使用するテクスチャを設定
			mesh->DrawSubset(c);								//	メッシュをレンダリングする
		}
	}
	bool cMesh::HitCheck_BBox_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_)
	{
		ML::Vec3  rayPos = rp_;
		ML::Vec3  rayDir = rd_;

		//デフォルト変換行列の逆行列でレイを座標変換する
		//＊未実装

		ML::Vec3  rayPos2 = rayPos+rayDir;	//始点と終点の関係を入れ替えたレイ
		ML::Vec3  rayDir2 = -rayDir;

		//レイとＢボックスの接触判定
		if(TRUE == D3DXBoxBoundProbe(&bBox.f, &bBox.r, &rayPos, &rayDir)){
			if(TRUE == D3DXBoxBoundProbe(&bBox.f, &bBox.r, &rayPos2, &rayDir2)){
				return true;
			}
		}
		return false;
	}
	//	レイとメッシュの接触判定を行う
	bool cMesh::HitCheck_Mesh_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_,  float&  per1_)
	{
		ML::Vec3  rayPos = rp_;
		ML::Vec3  rayDir = rd_;

		//デフォルト変換行列の逆行列でレイを座標変換する
		//＊未実装

		BOOL  hit = FALSE;
		HRESULT  hr;
		float  u, v;
		hr = D3DXIntersect(mesh, &rayPos, &rayDir, &hit, NULL, &u, &v, &per1_, NULL, NULL);
		if(hr == D3D_OK && hit){	return  true;}
		return  false;
	}
	//------------------------------------------------------------
	cImage::cImage( const tstring&	fpath_,
					DWORD			color_)
	:	rot(0),
		rotC(0, 0)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	throw ERR_NOTDEVICE; }

		HRESULT hRet;
	//	画像ファイルをメモリにロード
		hRet = D3DXCreateTextureFromFileEx(	dgi->Device( ), 
											fpath_.c_str( ),
											0, 0, D3DX_FROM_FILE, 0,
											D3DFMT_A8R8G8B8,
											D3DPOOL_DEFAULT,
											D3DX_FILTER_NONE,
											D3DX_FILTER_NONE,
											color_,
											NULL,
											NULL,
											&tex);
		if(hRet != D3D_OK){	
			MessageBox(NULL, fpath_.c_str( ), _T("画像の読み込みに失敗しました"), MB_OK);
			throw ERR_CREATE; 
		}
	}
	cImage::~cImage( )
	{
		ML::SafeRelease(&tex);
	}
//	画像を生成する
	cImage::SP cImage::Create(	const tstring&	fpath_,
								DWORD			color_)
	{
		return cImage::SP( new cImage(fpath_, color_) );
	}
//	画像を表示する
	void cImage::Draw(	const RECT&		draw,	//	描画先
						const RECT&		src,	//	転送元
						DWORD			color)	//	不透明度＆色
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	return; }

		float	fSrcW = float(src.right - src.left),
				fSrcH = float(src.bottom - src.top),
				fDrawW = float(draw.right - draw.left),
				fDrawH = float(draw.bottom - draw.top);

		D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
		ML::Vec3 ptCenter(0.0f, 0.0f, 0.0f);						//	描画の基準値の設定
		ML::Vec3 position(0.0f, 0.0f, 0.0f);						//	表示する位置を指定
		D3DXVECTOR2 drawPos(float(draw.left), float(draw.top));		//	描画先座標（演算前）
		D3DXVECTOR2 scale(fDrawW / fSrcW, fDrawH / fSrcH);			//	スケーリング係数（倍率を指定）

		D3DXMatrixTransformation2D(	&matDraw, NULL, 0.0f, &scale,
									&rotC, rot, &drawPos);
		dgi->Sprite( )->SetTransform(&matDraw);
		dgi->Sprite( )->Draw(tex, &src, &ptCenter, &position, color);
	}
	void cImage::Rotation(	float				r_,		//	回転量（ラジアン）
							const D3DXVECTOR2&	c_)		//	回転軸（draw相対座標）
	{
		rot = r_;
		rotC = c_;
	}
	//------------------------------------------------------------
	cFont::cFont(	const tstring&		fname_,
					WORD				width_,
					WORD				height_,
					WORD				weight_,
					BYTE				charSet_)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	throw ERR_NOTDEVICE; }

		D3DXFONT_DESC dxfd = 
		{
			height_, width_, weight_, 0,
			0, charSet_, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE
		};

		HRESULT hRet;
	//	フォント生成
		hRet = D3DXCreateFontIndirect(dgi->Device( ), &dxfd, &font);
		if(hRet != D3D_OK){	throw ERR_CREATE; }
	}

	cFont::~cFont( )
	{
		ML::SafeRelease(&font);
	}
	cFont::SP cFont::Create(	const tstring&	fname_,
								WORD			width_,
								WORD			height_, 
								WORD			weight_,
								BYTE			charSet_)
	{
		return cFont::SP( new cFont(fname_, width_, height_, weight_, charSet_) );
	}
	//	文字列を表示する
	void cFont::Draw(	const RECT&		draw_,	//	描画先
						const tstring&	tex_,
						DWORD			color_,
						UINT			uFormat_)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	return; }

		D3DXMATRIX matDraw;
		D3DXMatrixIdentity(&matDraw);
		dgi->Sprite( )->SetTransform(&matDraw);
		RECT d = draw_;
		font->DrawText(dgi->Sprite( ), tex_.c_str( ), tex_.length( ), &d, uFormat_, color_);
	}
	//	フレーム付き文字列を表示する
	void cFont::DrawF(	const RECT&		draw_,	//	描画先
						const tstring&	tex_,
						FRAME			mode_,
						DWORD			color_,
						DWORD			fColor_,
						UINT			uFormat_)
	{
		if(mode_ == x1)
		{
			RECT d;
			d = draw_; OffsetRect(&d, +1, +1);	Draw(d, tex_, fColor_, uFormat_);
			Draw(draw_, tex_, color_, uFormat_);
		}
		else if(mode_ == x2)
		{
			RECT d;
			d = draw_; OffsetRect(&d, -1, -1);	Draw(d, tex_, fColor_, uFormat_);
			d = draw_; OffsetRect(&d, +1, +1);	Draw(d, tex_, fColor_, uFormat_);
			Draw(draw_, tex_, color_, uFormat_);
		}
		else if(mode_ == x4)
		{
			RECT d;
			d = draw_; OffsetRect(&d, -1, 0);	Draw(d, tex_, fColor_, uFormat_);
			d = draw_; OffsetRect(&d, +1, 0);	Draw(d, tex_, fColor_, uFormat_);
			d = draw_; OffsetRect(&d, 0, -1);	Draw(d, tex_, fColor_, uFormat_);
			d = draw_; OffsetRect(&d, 0, +1);	Draw(d, tex_, fColor_, uFormat_);
			Draw(draw_, tex_, color_, uFormat_);
		}
	}

	//------------------------------------------------------------
	cCamera::cCamera(	const ML::Vec3&	tg_,	//	被写体の位置
						const ML::Vec3&	pos_,	//	カメラの位置
						const ML::Vec3&	up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
						float				fov_,	//	視野角
						float				np_,	//	前クリップ平面（これより前は映らない）
						float				fp_,	//	後クリップ平面（これより後ろは映らない）
						float				asp_)	//	アスペクト比（画面の比率に合わせる　横÷縦）	
	: pos(pos_), target(tg_), up(up_), fov(fov_), nearPlane(np_), forePlane(fp_), aspect(asp_)
	{
		UpDate( );
	}
	cCamera::~cCamera( )
	{
	}
//	カメラを生成する
	cCamera::SP cCamera::Create(	const ML::Vec3&	tg_,	//	被写体の位置
									const ML::Vec3&	pos_,	//	カメラの位置
									const ML::Vec3&	up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
									float				fov_,	//	視野角
									float				np_,	//	前クリップ平面（これより前は映らない）
									float				fp_,	//	後クリップ平面（これより後ろは映らない）
									float				asp_)	//	アスペクト比（画面の比率に合わせる　横÷縦）	
	{
		return cCamera::SP( new cCamera(tg_, pos_, up_, fov_, np_, fp_, asp_) );
	}
	//	カメラの設定
	void cCamera::UpDate( )
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	return; }

	//ビュー行列を設定
		D3DXMatrixLookAtLH(	&matView, &pos, &target, &up);
		dgi->Device( )->SetTransform(D3DTS_VIEW, &matView);

	//プロジェクション行列を設定
		D3DXMatrixPerspectiveFovLH(	&matProj, fov, aspect, nearPlane, forePlane);
		dgi->Device( )->SetTransform(D3DTS_PROJECTION, &matProj);

	}
}
