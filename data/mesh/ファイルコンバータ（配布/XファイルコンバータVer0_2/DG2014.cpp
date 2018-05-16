#include "DG2014.h"
#include "MyPG.h"

//-------------------------------------------------------------
#define kFileHeader "3DMesh YS Ver0.2"

struct sFileHeader
{
	char  Kind[32];
};
//�}�e���A���R���e�i
class cMaterialContainerS{
public:
	char					name[64];	//�}�e���A����
	ML::Color				diffuse;	//�}�e���A�����:�f�t���[�Y
	ML::Color				ambient;	//�}�e���A�����:�A���r�G���g
	ML::Color				speculer;	//�}�e���A�����:�X�y�L����
	ML::Color				emissive;	//�}�e���A�����:�G�~�b�V�u
	char					texName[64];//�}�e���A����
	char					bw[320];//���g�p�G���A�i���v512�o�C�g�j
};
class cMeshGroupS{
public:
	DWORD					mcNumber;	//�Ή�����}�e���A���ԍ�
	DWORD					indexNum;	//�C���f�b�N�X��
	vector<DWORD>			vIndex;		//�C���f�b�N�X
};

vector<cMaterialContainerS>	mateConts;
vector<cMeshGroupS> meshGroups;


struct sVertex{	D3DXVECTOR3  v, n;	D3DXVECTOR2 t;};

namespace DG
{
	bool  SaveFileName(tstring&  fpath_)
	{
		//	�t�@�C����ۑ����鏀�������܂�
		HANDLE			file			= NULL;
		OPENFILENAME	openFileName	= { ( DWORD )0 };
		char			filePath[256]	= "";
		strcpy(filePath,  fpath_.c_str( ));
		char			fileTitle[64]	= "";
		//�@�\���̂̐ݒ�����܂�
		ZeroMemory( &openFileName, sizeof( openFileName ) );
		openFileName.lStructSize		= sizeof( openFileName );
		openFileName.lpstrTitle			= "�ۑ�����SOB�t�@�C�������w�肵�ĉ�����";
		openFileName.hwndOwner			= ge->wh;
		openFileName.lpstrFilter		= "SOB̧��(*.SOB)\0\0";
		openFileName.nFilterIndex		= 1;
		openFileName.lpstrFile			= filePath;
		openFileName.nMaxFile			= sizeof( filePath );
		openFileName.lpstrFileTitle		= fileTitle;
		openFileName.nMaxFileTitle		= sizeof( fileTitle );
		openFileName.lpstrDefExt		= "SOB";
		openFileName.Flags				=
			OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST |
			OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;// | OFN_ALLOWMULTISELECT;
		//	�ۑ�����t�@�C���̃p�X���擾
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
		//���b�V�����璸�_���𒸂�
		IDirect3DVertexBuffer9*   vb;
		this->mesh->GetVertexBuffer(&vb);
		//���_�t�H�[�}�b�g����A�P���_������̃f�[�^�T�C�Y���擾����
		D3DVERTEXBUFFER_DESC vbDesc;
		vb->GetDesc(&vbDesc);
		DWORD  vertexSize  =  D3DXGetFVFVertexSize( vbDesc.FVF );
		//���_�o�b�t�@�̃T�C�Y����A���_�������߂�
		DWORD  numVertex  =  vbDesc.Size  /  vertexSize;
		//���_���ɁA���_���W�E�@���EUV���܂܂�邩���ׂ�
		bool  bPos,  bNormal,  bUV;
		bPos	=  (vbDesc.FVF  &  D3DFVF_XYZ) ? true : false;
		bNormal	=  (vbDesc.FVF  &  D3DFVF_NORMAL) ? true : false;
		bUV		=  (vbDesc.FVF  &  D3DFVF_TEX1) ? true : false;
		if(!bPos  ||  !bNormal  ||  !bUV){
			return false;
		}
		//�e��񂪁A�擪���牽�o�C�g�ڂɂ��邩���ׂ�
		DWORD  posPos		=  D3DXGetFVFVertexSize( (vbDesc.FVF % D3DFVF_XYZ) );
		DWORD  normalPos	=  D3DXGetFVFVertexSize( (vbDesc.FVF % D3DFVF_NORMAL) );
		DWORD  uvPos		=  D3DXGetFVFVertexSize( (vbDesc.FVF % D3DFVF_TEX1) );
		//�S���_���󂯎���o�b�t�@������
		vector<ML::Vec3>  vCoord;		vCoord.reserve(numVertex);
		vector<ML::Vec3>  vNormal;		vNormal.reserve(numVertex);
		vector<ML::Vec2>  vUV;			vUV.reserve(numVertex);
		
		//�o�[�e�b�N�X�o�b�t�@����f�[�^������邿��邷��
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
		//�C���f�b�N�X�o�b�t�@�ɃA�N�Z�X
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
		//�C���f�b�N�X��S���[�߂���o�b�t�@�����
		vector<DWORD>  index;		index.reserve(numIndex);
		//�C���f�b�N�X�����b�V�����炿��邿���
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
		//�������ɃA�N�Z�X
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
		//	�}�e���A�����o�b�t�@�ւ̃|�C���^�擾
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
		//�����A�����͂���ŏI��

		//�t�@�C���ɏo�͂��܂���`
		FILE *fpW;
		//
		fopen_s(&fpW, fpath_.c_str( ),"wb");

		//�t�@�C���w�b�_
		sFileHeader  kind = {kFileHeader};
		fwrite(&kind, sizeof(kind), 1, fpW);

		//�}�e���A���R���e�i�����܂���
		DWORD  mc = mg.size( );
		fwrite(&mc, sizeof(mc), 1, fpW);
		fwrite(&mg[0], sizeof(cMaterialContainerS), mg.size( ), fpW);

		//���b�V���O���[�v�����܂���
		DWORD  mgs = ms.size( );
		fwrite(&mgs, sizeof(mgs), 1, fpW);//����
		for(DWORD  c = 0; c < ms.size( ); c++){
			fwrite(&ms[c].mcNumber, sizeof(DWORD), 1, fpW);//�Ή��}�e���A���ԍ�
			fwrite(&ms[c].indexNum, sizeof(DWORD), 1, fpW);//�C���f�b�N�X��
			DWORD tm =  ms[c].vIndex.size( );
			fwrite(&ms[c].vIndex[0], sizeof(DWORD), ms[c].vIndex.size( ), fpW);//�C���f�b�N�X�z��
		}

		//���߂̓o�[�e�b�N�X�ł���
		char  vcTitle[16] = "POSITION";
		fwrite(vcTitle, sizeof(vcTitle), 1, fpW);//�w�b�_��
		DWORD  vc = vCoord.size( );
		fwrite(&vc, sizeof(vc), 1, fpW);//����
		fwrite(&vCoord[0], sizeof(ML::Vec3), vCoord.size( ), fpW);//�f�[�^


		char  ncTitle[16] = "NORMAL";
		fwrite(ncTitle, sizeof(ncTitle), 1, fpW);//�w�b�_��
		DWORD  nc = vNormal.size( );
		fwrite(&nc, sizeof(nc), 1, fpW);//����
		fwrite(&vNormal[0], sizeof(ML::Vec3), vNormal.size( ), fpW);//�f�[�^


		char  tcTitle[16] = "TEX_UV";
		fwrite(tcTitle, sizeof(tcTitle), 1, fpW);//�w�b�_��
		DWORD  tc = vUV.size( );
		fwrite(&tc, sizeof(tc), 1, fpW);//����
		fwrite(&vUV[0], sizeof(ML::Vec2), vUV.size( ), fpW);//�f�[�^


		fclose(fpW);	
		return true;
	}

}
//-------------------------------------------------------------







//����������������������������������������������������������������������������������������������
//���摜����n�@�\�Q																		�@��
//����������������������������������������������������������������������������������������������
namespace DG
{
	cObj::WP cObj::winst;	//	�P�������݂ł��Ȃ��C���X�^���X

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
	//	Direct3D�I�u�W�F�N�g�̐���
		d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if(d3d == 0){	throw ERR_CREATE;	}
	//	�f�o�C�X�̔\�͂��m�F
		ZeroMemory(&caps, sizeof(caps));
		d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	//	�f�o�C�X�̐ݒ�
		ZeroMemory(&pp, sizeof(pp));
		//	�𑜓x�Z�b�g
		pp.BackBufferWidth = w_;
		pp.BackBufferHeight = h_;
		if(sm_ == true){
		// �E�C���h�E���[�h�ł͂Ȃ�
			pp.Windowed = FALSE;
		}
		else{
		//	�E�C���h�E���[�h
			pp.Windowed = TRUE;
		}

	//	�o�b�N�o�b�t�@�A�����_�����O���
		pp.BackBufferFormat = D3DFMT_X8R8G8B8;
		pp.BackBufferCount = 2;
		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_FLIP;
	//	�X�e���V���o�b�t�@��L���Ɂi�t�H�[�}�b�g��16�r�b�g�t�H�[�}�b�g�j
		pp.EnableAutoDepthStencil = TRUE;
		pp.AutoDepthStencilFormat = D3DFMT_D16;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

		//	�n�[�h�E�F�A�@�\�I��p�̏��
		struct{	_D3DDEVTYPE dt; DWORD vp; }modeTable[3] = {
			{D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING},	//	T&L HAL
			{D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING},	//	HAL
			{D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING}	//	REF
		};
	//	�f�o�C�X�̍쐬
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
	//	�f�o�C�X�������s
		if(devFlag == TRUE){	d3d->Release( ); throw ERR_CREATE;}
	//	�����_�����O���̖@�����K��ON
		d3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	//	�X�v���C�g�����_�����O�I�u�W�F�N�g�̐���
		if(FAILED(D3DXCreateSprite(d3dDevice, &sprite))){	throw ERR_CREATE;}
	}
	cObj::~cObj( )
	{
	//	�X�v���C�g�̉��
		ML::SafeRelease(&sprite);
	//	D3D�f�o�C�X�̉��
		ML::SafeRelease(&d3dDevice);
	//	D3D�I�u�W�F�N�g�̉��
		ML::SafeRelease(&d3d);
	}
	//	DG���C�u�����𐶐�����
	cObj::SP cObj::Create(	HWND		hw_,	//	�E�B���h�E�n���h��
							int			w_,		//	����
							int			h_,		//	�c��
							bool		sm_)	//	�X�N���[�����[�h
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
//	��ʕ`��i�J�n�j
	BOOL cObj::Begin(	DWORD color_)
	{
	//	�o�b�N�o�b�t�@���N���A�i��ʏ���
		if( FAILED( d3dDevice->Clear(	0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
									color_, 1.0f, 0) ) ){return false;}
	//	�`��J�n�������
		if( FAILED( d3dDevice->BeginScene( ) ) ){return false;}
		return true;
	}
//	3D�`��̋��𓾂�
	void cObj::Begin3D( )
	{
	}
//	3D�`����I������
	void  cObj::End3D( )
	{
	}
//	2D�`��̋��𓾂�
	void  cObj::Begin2D( )
	{
		sprite->Begin(D3DXSPRITE_ALPHABLEND);
	}
//	2D�`��̂��I������
	void  cObj::End2D( )
	{
		sprite->End( );
	}
//	��ʕ`��i�I���j
	void  cObj::End( )
	{
	//	�`��I��
		d3dDevice->EndScene( );
	//	�o�b�N�o�b�t�@���v���C�}���o�b�t�@�ɃR�s�[ or �t���b�v
		if (FAILED(d3dDevice->Present(NULL, NULL, NULL, NULL))){
			d3dDevice->Reset(NULL);
		}
	}
	//	�f�o�C�X
	LPDIRECT3DDEVICE9 cObj::Device( ){	return d3dDevice;	}
	LPD3DXSPRITE cObj::Sprite( ){		return sprite;	}
	//------------------------------------------------------------
	cMesh::cMesh(	const tstring&		fpath_,
					const D3DXMATRIX&	matDef_)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	throw ERR_NOTDEVICE; }

	//	����������������������������������������������������������������������
	//	�w�t�@�C�����烁�b�V���ƃ}�e���A�������擾
		if(FAILED(	D3DXLoadMeshFromX(	fpath_.c_str( ),
										D3DXMESH_MANAGED,
										dgi->Device( ),
										NULL,
										&materials,
										NULL,
										&numMaterials,
										&mesh))){
			MessageBox(NULL, fpath_.c_str( ), _T("���b�V���̓ǂݍ��݂Ɏ��s���܂���"), MB_OK);
			throw ERR_CREATE;
		}
	//	����������������������������������������������������������������������
	//	�ǂݍ���X�t�@�C���̃t�H�[�}�b�g�𓾂�
		DWORD fvf = mesh->GetFVF();

	//	����������������������������������������������������������������������
	//	�}�e���A�����ɑ΂��鏈��
		tstring path = ML::GetPath(fpath_);
		if(materials != NULL){
		//	�}�e���A�����o�b�t�@�ւ̃|�C���^�擾
			D3DXMATERIAL* material = (D3DXMATERIAL*)materials->GetBufferPointer( );
		//	�e�N�X�`���p�̗̈���쐬
			textures = new LPDIRECT3DTEXTURE9[numMaterials];
		//	�}�e���A����񂩂�u�F���v�Ɓu�e�N�X�`���t�@�C�����v���擾
			for(DWORD c = 0; c < numMaterials; c++){
				D3DMATERIAL9* mat = &material[c].MatD3D;
			//	�}�e���A�����ɃA���r�G���g�l���Ȃ��ꍇ�A�����I�ɐݒ肷��
				if(	mat->Ambient.r == 0.0f && mat->Ambient.g == 0.0f &&
					mat->Ambient.b == 0.0f ){
						mat->Ambient.a = 1.0f;	mat->Ambient.r = 1.0f;
						mat->Ambient.g = 1.0f;	mat->Ambient.b = 1.0f;
					}
			//	�e�N�X�`���t�@�C����������ꍇ�A�e�N�X�`����ǂݍ���
				if(material[c].pTextureFilename != NULL){
					tstringstream ss;
					ss << path << _T("/") << material[c].pTextureFilename;
					tstring tfp;
					ss >> tfp;

					if(FAILED( D3DXCreateTextureFromFile( 
						dgi->Device( ), tfp.c_str( ), &textures[c]) ) ){
						MessageBox(NULL, tfp.c_str( ), _T("�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���"), MB_OK);
						textures[c] = NULL;
					}
				}
			}
		//	����������������������������������������������������������������������
		//	X�t�@�C���̃��b�V�������A�{�v���O�����p�ɕϊ�
		//	�N���[���̐ݒ�
			D3DVERTEXELEMENT9 decl[] = 
			{
				{	0,	0,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0},
				{	0, 12,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0},	
				{	0, 24,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0},
				D3DDECL_END( )
			//	0         1           2       3
			//	01234567890123456789012345678901
			//  X---Y---Z---X---Y---Z---U---V---
			//  ���_���W----�@��Vec-----tex���W
			//	�܂�A���_���W�A�@���x�N�g���A�e�N�X�`�����W���������N���[���𐶐�����ׂ̐ݒ�
			};
		//	�N���[���𐶐�����
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

		//	�N���[�����c���A�I���W�i�����������
			pOld->Release( );

		//	X�t�@�C������ǂݍ��񂾃��b�V���ɖ@����񂪖����ꍇ
			if(!(fvf & D3DFVF_NORMAL)){	D3DXComputeNormals(mesh, NULL);}
		//	�f�t�H���g�ϊ��s��̐ݒ�
			D3DXMATRIX md;
			D3DXMatrixIdentity(&md);
			matDef = matDef_;
		}

		//���b�V������o�E���f�B���O�{�b�N�X�𐶐�����
		///�o�[�e�b�N�X�o�b�t�@�̃��b�N
		void*  vtBP;
		mesh->LockVertexBuffer(D3DLOCK_READONLY,  &vtBP);
		///���_���̎擾
		DWORD  vtNum  =  mesh->GetNumVertices( );
		//���_�P�ʂ̃o�C�g���̎擾
		DWORD  vtSize =  D3DXGetFVFVertexSize( mesh->GetFVF( ) );

		///�o�E���f�B���O�{�b�N�X�̐���
		D3DXComputeBoundingBox((LPD3DXVECTOR3)vtBP,  vtNum,  vtSize,  &bBox.f,  &bBox.r);

		///�o�[�e�b�N�X�o�b�t�@�̃A�����b�N
		mesh->UnlockVertexBuffer( );
		vtBP = nullptr;
	}
	cMesh::~cMesh( )
	{
	//	�e�N�X�`���e�[�u�����J������
		if(textures != NULL){
		//	�X�̃e�N�X�`�����J������
			for(DWORD c = 0; c < numMaterials; c++){
				ML::SafeRelease(&textures[c]);
			}
		//	�e�[�u�����J��
			delete[] textures;
			textures = NULL;
		}
	//	�}�e���A�����J������
		ML::SafeRelease(&materials);
	//	���b�V�����J������
		ML::SafeRelease(&mesh);
	}
	//	���b�V���𐶐�����
	cMesh::SP cMesh::Create(	const tstring&		fpath_,
								const D3DXMATRIX&	matDef_)
	{
		return SP( new cMesh(fpath_, matDef_) );
	}
	//	���b�V����\������
	void cMesh::Draw(	const ML::Vec3&	pos_,		//	�\�����W
						float				rotY_)		//	��]�p�x�i���W�A��)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	throw ERR_NOTDEVICE; }
		D3DXMATRIX matW, matRY, matT;
	//	Y���ɑ΂����]�s����쐬����
		D3DXMatrixRotationY(&matRY, rotY_);
	//	���s�ړ��s����쐬����
		D3DXMatrixTranslation(&matT, pos_.x, pos_.y, pos_.z);
	//	�s��̍���
		matW = matRY * matT;
	//	���[���h�ϊ��s���ݒ肷��
		dgi->Device( )->SetTransform(D3DTS_WORLD, &matW);
		
	//	���b�V�����}�e���A�����ɕ`��
		D3DXMATERIAL* material = (D3DXMATERIAL*)materials->GetBufferPointer( );
		for(DWORD c = 0; c < numMaterials; c++){
			dgi->Device( )->SetMaterial(&material[c].MatD3D);	//	�����_�����O�Ɏg�p����}�e���A����ݒ�
			if(textures){
				dgi->Device( )->SetTexture(0, textures[c]);		//	�����_�����O�Ɏg�p����e�N�X�`����ݒ�
			}
			else{
				dgi->Device( )->SetTexture(0, nullptr);		//	�����_�����O�Ɏg�p����e�N�X�`����ݒ�
			}
			mesh->DrawSubset(c);								//	���b�V���������_�����O����
		}
	}
	//	���b�V����\������
	void cMesh::Draw(	const D3DXMATRIX&	mat_)
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	throw ERR_NOTDEVICE; }
	//	���[���h�ϊ��s���ݒ肷��
		dgi->Device( )->SetTransform(D3DTS_WORLD, &mat_);
		
	//	���b�V�����}�e���A�����ɕ`��
		D3DXMATERIAL* material = (D3DXMATERIAL*)materials->GetBufferPointer( );
		for(DWORD c = 0; c < numMaterials; c++){
			dgi->Device( )->SetMaterial(&material[c].MatD3D);	//	�����_�����O�Ɏg�p����}�e���A����ݒ�
			dgi->Device( )->SetTexture(0, textures[c]);		//	�����_�����O�Ɏg�p����e�N�X�`����ݒ�
			mesh->DrawSubset(c);								//	���b�V���������_�����O����
		}
	}
	bool cMesh::HitCheck_BBox_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_)
	{
		ML::Vec3  rayPos = rp_;
		ML::Vec3  rayDir = rd_;

		//�f�t�H���g�ϊ��s��̋t�s��Ń��C�����W�ϊ�����
		//��������

		ML::Vec3  rayPos2 = rayPos+rayDir;	//�n�_�ƏI�_�̊֌W�����ւ������C
		ML::Vec3  rayDir2 = -rayDir;

		//���C�Ƃa�{�b�N�X�̐ڐG����
		if(TRUE == D3DXBoxBoundProbe(&bBox.f, &bBox.r, &rayPos, &rayDir)){
			if(TRUE == D3DXBoxBoundProbe(&bBox.f, &bBox.r, &rayPos2, &rayDir2)){
				return true;
			}
		}
		return false;
	}
	//	���C�ƃ��b�V���̐ڐG������s��
	bool cMesh::HitCheck_Mesh_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_,  float&  per1_)
	{
		ML::Vec3  rayPos = rp_;
		ML::Vec3  rayDir = rd_;

		//�f�t�H���g�ϊ��s��̋t�s��Ń��C�����W�ϊ�����
		//��������

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
	//	�摜�t�@�C�����������Ƀ��[�h
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
			MessageBox(NULL, fpath_.c_str( ), _T("�摜�̓ǂݍ��݂Ɏ��s���܂���"), MB_OK);
			throw ERR_CREATE; 
		}
	}
	cImage::~cImage( )
	{
		ML::SafeRelease(&tex);
	}
//	�摜�𐶐�����
	cImage::SP cImage::Create(	const tstring&	fpath_,
								DWORD			color_)
	{
		return cImage::SP( new cImage(fpath_, color_) );
	}
//	�摜��\������
	void cImage::Draw(	const RECT&		draw,	//	�`���
						const RECT&		src,	//	�]����
						DWORD			color)	//	�s�����x���F
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	return; }

		float	fSrcW = float(src.right - src.left),
				fSrcH = float(src.bottom - src.top),
				fDrawW = float(draw.right - draw.left),
				fDrawH = float(draw.bottom - draw.top);

		D3DXMATRIX matDraw;	//	���W�ϊ��}�g���b�N�X�̊i�[��
		ML::Vec3 ptCenter(0.0f, 0.0f, 0.0f);						//	�`��̊�l�̐ݒ�
		ML::Vec3 position(0.0f, 0.0f, 0.0f);						//	�\������ʒu���w��
		D3DXVECTOR2 drawPos(float(draw.left), float(draw.top));		//	�`�����W�i���Z�O�j
		D3DXVECTOR2 scale(fDrawW / fSrcW, fDrawH / fSrcH);			//	�X�P�[�����O�W���i�{�����w��j

		D3DXMatrixTransformation2D(	&matDraw, NULL, 0.0f, &scale,
									&rotC, rot, &drawPos);
		dgi->Sprite( )->SetTransform(&matDraw);
		dgi->Sprite( )->Draw(tex, &src, &ptCenter, &position, color);
	}
	void cImage::Rotation(	float				r_,		//	��]�ʁi���W�A���j
							const D3DXVECTOR2&	c_)		//	��]���idraw���΍��W�j
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
	//	�t�H���g����
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
	//	�������\������
	void cFont::Draw(	const RECT&		draw_,	//	�`���
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
	//	�t���[���t���������\������
	void cFont::DrawF(	const RECT&		draw_,	//	�`���
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
	cCamera::cCamera(	const ML::Vec3&	tg_,	//	��ʑ̂̈ʒu
						const ML::Vec3&	pos_,	//	�J�����̈ʒu
						const ML::Vec3&	up_,	//	�J�����̏�����������x�N�g���i��̂x�{�Œ�j
						float				fov_,	//	����p
						float				np_,	//	�O�N���b�v���ʁi������O�͉f��Ȃ��j
						float				fp_,	//	��N���b�v���ʁi��������͉f��Ȃ��j
						float				asp_)	//	�A�X�y�N�g��i��ʂ̔䗦�ɍ��킹��@�����c�j	
	: pos(pos_), target(tg_), up(up_), fov(fov_), nearPlane(np_), forePlane(fp_), aspect(asp_)
	{
		UpDate( );
	}
	cCamera::~cCamera( )
	{
	}
//	�J�����𐶐�����
	cCamera::SP cCamera::Create(	const ML::Vec3&	tg_,	//	��ʑ̂̈ʒu
									const ML::Vec3&	pos_,	//	�J�����̈ʒu
									const ML::Vec3&	up_,	//	�J�����̏�����������x�N�g���i��̂x�{�Œ�j
									float				fov_,	//	����p
									float				np_,	//	�O�N���b�v���ʁi������O�͉f��Ȃ��j
									float				fp_,	//	��N���b�v���ʁi��������͉f��Ȃ��j
									float				asp_)	//	�A�X�y�N�g��i��ʂ̔䗦�ɍ��킹��@�����c�j	
	{
		return cCamera::SP( new cCamera(tg_, pos_, up_, fov_, np_, fp_, asp_) );
	}
	//	�J�����̐ݒ�
	void cCamera::UpDate( )
	{
		DG::cObj::SP dgi = DG::cObj::GetInst( );
		if(dgi == 0){	return; }

	//�r���[�s���ݒ�
		D3DXMatrixLookAtLH(	&matView, &pos, &target, &up);
		dgi->Device( )->SetTransform(D3DTS_VIEW, &matView);

	//�v���W�F�N�V�����s���ݒ�
		D3DXMatrixPerspectiveFovLH(	&matProj, fov, aspect, nearPlane, forePlane);
		dgi->Device( )->SetTransform(D3DTS_PROJECTION, &matProj);

	}
}
