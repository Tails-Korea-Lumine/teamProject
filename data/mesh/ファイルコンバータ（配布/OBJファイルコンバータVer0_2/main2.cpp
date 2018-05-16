#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
//#include <atlstr.h>
#include "myLib.h"

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


struct sFace{	DWORD  v, t, n;};
struct sVertex{	D3DXVECTOR3  v, n;	D3DXVECTOR2 t;};


bool  ObjToSOB(char  fpathR_[], char  fpathW_[]); 	
//ファイルチェック
bool OBJFileSub_FileCheck(FILE*  fp_, int&  usemtlNum_, string&  matFile_);
//OBJファイル用マテリアルファイルからマテリアルを読み込む
bool LoadMaterial_FromOBJFile(const tstring&		fpath_);
//デフォルトマテリアルコンテナとメッシュグループを作る（マテリアルの無いファイル）
void OBJFileSub_DefMaterialContainer_MeshGroup( );
//OBJファイルから頂点情報を読み出す
void OBJFileSub_ReadVertex(FILE*  fp_, 
	vector<D3DXVECTOR3>& v_, vector<D3DXVECTOR3>& n_, vector<D3DXVECTOR2>& t_);
//OBJファイルからフェイス情報を読み出す
void OBJFileSub_ReadFace(FILE*  fp_, vector<pair<tstring, vector<sFace>>>& f_);
//フェイス展開
void OBJFileSub_FaceVertex( vector<pair<tstring, vector<sFace>>>& f_, 
							vector<D3DXVECTOR3>& v_, vector<D3DXVECTOR3>& n_, vector<D3DXVECTOR2>& t_,
							vector<sVertex>& vertex_);
//重複する頂点を取り除く
void OBJFileSub_VertexRemove(	vector<sVertex>& base_, 	vector<sVertex>& out_);
//全体に対するインデックスを生成
void OBJFileSub_CreateIndexAll(vector<DWORD>& index_, vector<sVertex>& base_, vector<sVertex>& ex_);

void main( int argc, char *argv[ ])
{
	int rtv;
	printf("OBJ形式ファイルを変換する。Ver0.1\n\n");
	//ファイル名を取得
	if(argc < 2){
		printf("変換するファイルが指定されていません\n");
		getch( );
		return;
	}
	//変換対象のファイル名を表示
	char fileName[_MAX_PATH];
	strcpy(fileName, argv[1]);
	printf("以下のファイルを変換します。\n");
	printf("%s\n",fileName);

	char fileName2[_MAX_PATH];
	strcpy(fileName2, argv[1]);
	int len = (int)strlen(fileName2);
	strcpy(&fileName2[len - 3], "SOB");
	//変換後のファイル名を表示
	printf("以下のファイルを作成します。\n");
	printf("%s\n",fileName2);
	//変換処理実行の確認
	while(true)
	{
		printf("ファイルの変換を開始してもよろしいですか？(Y/N)\n");
		rtv = getch( );
		if(rtv == 'Y' || rtv == 'y'){ break;}
		if(rtv == 'N' || rtv == 'n'){
			printf("変換処理を中止しました\n");
			getch( );
			return;
		}
	}
	if(false == ObjToSOB(fileName, fileName2)){
		printf("読み込み処理中にエラーが起こりました\n");
		_getch( );
		return;
	}
	//ファイル読み込み結果のレポート
	printf("変換処理が終了しました\n");
	_getch( );
}

//ファイルチェック
bool OBJFileSub_FileCheck(FILE*  fp_, int&  usemtlNum_, string&  matFile_)
{
	//マテリアルの有無及びフェイスグループ数を検出
	fseek(fp_,SEEK_SET,0);
	bool  matFile=false;
	int usemtlCnt = 0;
	char data[256]={0};
	while(fgets(data, 256, fp_)){
		if(strncmp(data,"usemtl ", 7) == 0){
			usemtlCnt++;	
		}
		if(strncmp(data,"mtllib ", 7)==0){
			matFile = true;
			char  fn[256];
			sscanf_s(&data[7], "%s", fn, sizeof(fn));
//			matFile_ = CString(fn);
			matFile_ = fn;
		}
	}
	usemtlNum_ = usemtlCnt;
	//マテリアルグループが無いよ
	if(usemtlCnt == 0){
		ML::MsgBox(_T("usemtlがありません"));
	}
	if(matFile == false){
		ML::MsgBox(_T("mtllibがありません"));
		return false;
	}
	return true;
}
//OBJファイル用マテリアルファイルからマテリアルを読み込む
bool LoadMaterial_FromOBJFile(const tstring&		fpath_)
{
	//ファイルパスをマルチバイトに変換
	string  fn = ML::ToString(fpath_);
	//ファイルを開いて内容を読み込む
	FILE* fp=NULL;
	fopen_s(&fp, fn.c_str( ),"rt");


	char data[256]={0};
	
	vector<cMaterialContainerS>::iterator  it;
	D3DXVECTOR4 v(0,0,0,0);
	while(fgets(data, 256, fp)){
		//マテリアル名
		if(strncmp(data,"newmtl ", 7)==0){
			//マテリアル名が出てきたら領域を新設
			cMaterialContainerS  w={0};
			w.diffuse  = ML::Color(1,1,1,1);	//マテリアル情報:デフューズ
			w.ambient  = ML::Color(1,1,1,1);	//マテリアル情報:アンビエント
			w.speculer = ML::Color(0,0,0,0);	//マテリアル情報:スペキュラ
			w.emissive = ML::Color(0,0,0,0);	//マテリアル情報:エミッシブ
			mateConts.push_back(w);
			it = mateConts.end( );
			it--;
			//名称を登録
			sscanf_s(&data[7],"%s ",it->name,sizeof(it->name));
		}
		//Ka　アンビエント
		else
		if(strncmp(data,"Ka ", 3)==0){
			sscanf_s(&data[3],"%f %f %f",&v.x,&v.y,&v.z);
			it->ambient = ML::Color(1,v.x,v.y,v.z);
		}
		//Kd　ディフューズ
		else
		if(strncmp(data,"Kd ", 3)==0){
			sscanf_s(&data[3],"%f %f %f",&v.x,&v.y,&v.z);
			it->diffuse = ML::Color(1,v.x,v.y,v.z);
		}
		//Ks　スペキュラー
		else
		if(strncmp(data,"Ks ", 3)==0){
			sscanf_s(&data[3],"%f %f %f",&v.x,&v.y,&v.z);
			it->speculer = ML::Color(1,v.x,v.y,v.z);
		}
		//map_Kd　テクスチャファイル名
		else
		if(strncmp(data,"map_Kd ", 7)==0){
			sscanf_s(&data[7],"%s ",it->texName,sizeof(it->texName));
		}

	}
	fclose(fp);
	return true;
}
//デフォルトマテリアルコンテナとメッシュグループを作る（マテリアルの無いファイル）
void OBJFileSub_DefMaterialContainer_MeshGroup( )
{
	cMaterialContainerS  w = {0};
	w.diffuse  = ML::Color(1,1,1,1);	//マテリアル情報:デフューズ
	w.ambient  = ML::Color(1,1,1,1);	//マテリアル情報:アンビエント
	w.speculer = ML::Color(0,0,0,0);	//マテリアル情報:スペキュラ
	mateConts.push_back(w);

	cMeshGroupS  wmg = {0};
	meshGroups.push_back(wmg);
}
//OBJファイルから頂点情報を読み出す
void OBJFileSub_ReadVertex(FILE*  fp_, 
	vector<D3DXVECTOR3>& v_, vector<D3DXVECTOR3>& n_, vector<D3DXVECTOR2>& t_)
{
	//０件目はダミーデータ
	v_.push_back( D3DXVECTOR3(0,0,0) );
	n_.push_back( D3DXVECTOR3(0,0,0) );
	t_.push_back( D3DXVECTOR2(0,0) );
	//マテリアルの有無及びフェイスグループ数を検出
	fseek(fp_,SEEK_SET,0);
	char data[256];
	while(fgets(data, 256, fp_)){
		//頂点
		if(strncmp(data,"v ", 2)==0){
			D3DXVECTOR3  w;
			sscanf_s(&data[2], "%f %f %f", &w.x, &w.y, &w.z);
			v_.push_back(w);
		}
		//法線
		else
		if(strncmp(data,"vn ", 3)==0){
			D3DXVECTOR3  w;
			sscanf_s(&data[3], "%f %f %f", &w.x, &w.y, &w.z);
			n_.push_back(w);
		}
		//UV
		else
		if(strncmp(data,"vt ", 3)==0){
			D3DXVECTOR2  w;
			sscanf_s(&data[3], "%f %f", &w.x, &w.y);
			t_.push_back(w);
		}
	}
}
//OBJファイルからフェイス情報を読み出す
void OBJFileSub_ReadFace(FILE*  fp_, vector<pair<tstring, vector<sFace>>>& f_)
{
	//マテリアルの有無及びフェイスグループ数を検出
	fseek(fp_,SEEK_SET,0);
	char data[256];
	int mode=-1;
	//データの構成をチェック
	while(fgets(data, 256, fp_)){
		if(strncmp(data,"f ", 2)==0){
			sFace  wf[3] = {0};
			//頂点/UV/法線
			if(		9 == sscanf_s(&data[2], "%d/%d/%d %d/%d/%d %d/%d/%d ",	&wf[0].v,&wf[0].t,&wf[0].n,	&wf[1].v,&wf[1].t,&wf[1].n,	&wf[2].v,&wf[2].t,&wf[2].n)){	mode=0; break;}
			//頂点//法線
			else if(6 == sscanf_s(&data[2], "%d//%d %d//%d %d//%d ",		&wf[0].v,         &wf[0].n,	&wf[1].v,         &wf[1].n,	&wf[2].v,         &wf[2].n)){	mode=1; break;}
			//頂点/UV/
			else if(6 == sscanf_s(&data[2], "%d/%d/ %d/%d/ %d/%d/ ",		&wf[0].v,&wf[0].t,			&wf[1].v,&wf[1].t,			&wf[2].v,&wf[2].t)){			mode=2; break;}
			//頂点//
			else if(3 == sscanf_s(&data[2], "%d// %d// %d// ",				&wf[0].v,					&wf[1].v,					&wf[2].v)){						mode=3; break;}
		}
	}
	if(mode==-1){return;}

	//読み込み
	fseek(fp_,SEEK_SET,0);
	vector<pair<tstring, vector<sFace>>>::iterator  it;
	while(fgets(data, 256, fp_)){
		if(strncmp(data,"f ", 2)==0){
			sFace  wf[3] = {0};
			if(mode == 0){			//頂点/UV/法線
				sscanf_s(&data[2], "%d/%d/%d %d/%d/%d %d/%d/%d ",	&wf[0].v,&wf[0].t,&wf[0].n,	&wf[1].v,&wf[1].t,&wf[1].n,	&wf[2].v,&wf[2].t,&wf[2].n);
			}else if(mode == 1){	//頂点//法線
				sscanf_s(&data[2], "%d//%d %d//%d %d//%d ",			&wf[0].v,         &wf[0].n,	&wf[1].v,         &wf[1].n,	&wf[2].v,         &wf[2].n);
			}else if(mode == 2){	//頂点/UV/
				sscanf_s(&data[2], "%d/%d/ %d/%d/ %d/%d/ ",			&wf[0].v,&wf[0].t,			&wf[1].v,&wf[1].t,			&wf[2].v,&wf[2].t);
			}else if(mode == 3){	//頂点//
				sscanf_s(&data[2], "%d// %d// %d// ",				&wf[0].v,					&wf[1].v,					&wf[2].v);
			}
			for(int c=0; c<3; c++){
				it->second.push_back(wf[c]);
			}
		}
		else
		if(strncmp(data,"usemtl ", 7)==0){
			//マテリアル名が出てきたら領域を新設
			f_.resize( f_.size( ) + 1);
			it  =  f_.end( );
			it--;
			//名称を登録
			char n[256]={0};
			sscanf_s(&data[7],"%s ",n,sizeof(n));
			it->first = ML::ToTstring(n);
		}
	}
}

//フェイス展開
void OBJFileSub_FaceVertex( vector<pair<tstring, vector<sFace>>>& f_, 
							vector<D3DXVECTOR3>& v_, vector<D3DXVECTOR3>& n_, vector<D3DXVECTOR2>& t_,
							vector<sVertex>& vertex_)
{
	DWORD total = 0;
	for(auto  it = f_.begin( ); it != f_.end( ); it++){
		total += it->second.size( );
	}
	vertex_.resize(total);
	DWORD  vc=0;
	for(auto  it = f_.begin( ); it != f_.end( ); it++){
		for(DWORD  c = 0; c < it->second.size( ); c++, vc++){
			vertex_[vc].v = v_[	it->second[c].v ];
			vertex_[vc].n = n_[	it->second[c].n ];
			vertex_[vc].t = t_[	it->second[c].t ];
		}
	}
}
//重複する頂点を取り除く
void OBJFileSub_VertexRemove(	vector<sVertex>& base_, 	vector<sVertex>& out_)
{
	for(DWORD i=0; i<base_.size(); i++){
		bool f=true;
		for(DWORD j=0; j<out_.size(); j++){
			//既に同じものがあるとき
			if(	base_[i].v == out_[j].v &&
				base_[i].n == out_[j].n &&
				base_[i].t == out_[j].t ){
				f=false;
			}
		}
		//重複しなければ登録
		if(f){
			out_.push_back(base_[i]);
		}
	}
}
//全体に対するインデックスを生成
void OBJFileSub_CreateIndexAll(vector<DWORD>& index_, vector<sVertex>& base_, vector<sVertex>& ex_)
{
	index_.resize(base_.size());
	for(DWORD i=0; i<index_.size(); i++){
		bool f=true;
		for(DWORD j=0; j<ex_.size(); j++){
			//同じものを見つけたら
			if(	base_[i].v == ex_[j].v &&
				base_[i].n == ex_[j].n &&
				base_[i].t == ex_[j].t ){
				index_[i] = j;	//インデックス登録
				break;
			}
		}
	}
}






//OBJファイルを読み込み、別形式で保存する
bool  ObjToSOB(char  fpathR_[], char  fpathW_[])
{
	//読み込み側のファイルを開く
	FILE  *fpR;
	char data[256]={0};
	//ファイルを開いて内容を読み込む
	fopen_s(&fpR, fpathR_,"rt");

	//ファイルチェック
	int  usemtlNum;
	string  matFileName;
	if( OBJFileSub_FileCheck(fpR, usemtlNum, matFileName) ){
		//マテリアルが有る場合、マテリアルを読み込む
		tstring  mfp = ML::CreateFilePath(ML::ToTstring(fpathR_),ML::ToTstring(matFileName.c_str( )));
		LoadMaterial_FromOBJFile(mfp);
	}else{
		//マテリアルが無い場合、マテリアルを作る
		OBJFileSub_DefMaterialContainer_MeshGroup( );
	}

	//頂点情報の読み込み
	vector<D3DXVECTOR3>		vCoord;
	vector<D3DXVECTOR3>		vNormal;
	vector<D3DXVECTOR2>		vUV;
	OBJFileSub_ReadVertex(fpR, vCoord,  vNormal,  vUV);
	
	//フェイスを読み込み
	vector<pair<tstring, vector<sFace>>>	vFace;
	OBJFileSub_ReadFace(fpR, vFace);

	//ファイルから必要なデータはすべて読み込んだよ
	fclose(fpR);	

	//フェイス展開
	vector<sVertex>			vVertex;
	OBJFileSub_FaceVertex(vFace, vCoord, vNormal, vUV, vVertex);

	//重複する頂点情報を消去した頂点配列を生成
	vector<sVertex>			vVertex2;
	OBJFileSub_VertexRemove(vVertex, vVertex2);

	//全体に対するインデックスを生成
	vector<DWORD>	vIndex;
	OBJFileSub_CreateIndexAll(vIndex, vVertex, vVertex2);

	//各バッファに分離
	vector<D3DXVECTOR3>		vCoord2(vVertex2.size());
	vector<D3DXVECTOR3>		vNormal2(vVertex2.size());
	vector<D3DXVECTOR2>		vUV2(vVertex2.size());
	for(DWORD i=0; i<vVertex2.size(); i++){
		vCoord2[i] =	vVertex2[i].v;
		vNormal2[i] =	vVertex2[i].n;
		vUV2[i] =		vVertex2[i].t;
	}
	//インデックスも各グループに分離
	vector<pair<tstring, vector<DWORD>>>	vFace2(vFace.size());
	auto itIndexBase = vIndex.begin( );
	auto it = vFace.begin( );
	for(auto it2=vFace2.begin( ); it2 != vFace2.end(); it2++, it++){
		it2->first = it->first;
		it2->second.resize(it->second.size( ));
		for(DWORD  c = 0; c < it2->second.size( ); c++, itIndexBase++){
			it2->second[c] = (*itIndexBase);
		}
	}

	//メッシュグループ作るよ
	for(auto  it = vFace2.begin( ); it != vFace2.end( ); it++){
		cMeshGroupS wmg;
		//名前の一致するマテリアルを探せ
		for(DWORD c2 = 0; c2 < mateConts.size( );  c2++){
			if(it->first == ML::ToTstring(mateConts[c2].name)){
				wmg.mcNumber = c2;
			}
		}
		//インデックスを書き込む
		wmg.indexNum = it->second.size( );
		wmg.vIndex.resize(wmg.indexNum);
		for(DWORD  c3 = 0;  c3 != wmg.vIndex.size( );  c3++){
			wmg.vIndex[c3] = it->second[c3];
		}
		//マテリアルグループに設定
		meshGroups.push_back(wmg);
	}


	//ファイルに出力しますよ～
	FILE *fpW;
	//
	fopen_s(&fpW, fpathW_,"wb");

	//ファイルヘッダ
	sFileHeader  kind = {kFileHeader};
	fwrite(&kind, sizeof(kind), 1, fpW);

	//マテリアルコンテナ書きますよ
	DWORD  mc = mateConts.size( );
	fwrite(&mc, sizeof(mc), 1, fpW);
	fwrite(&mateConts[0], sizeof(cMaterialContainerS), mateConts.size( ), fpW);

	//メッシュグループ書きますよ
	DWORD  mg = meshGroups.size( );
	fwrite(&mg, sizeof(mg), 1, fpW);//件数
	for(DWORD  c = 0; c < meshGroups.size( ); c++){
		fwrite(&meshGroups[c].mcNumber, sizeof(DWORD), 1, fpW);//対応マテリアル番号
		fwrite(&meshGroups[c].indexNum, sizeof(DWORD), 1, fpW);//インデックス数
		DWORD tm =  meshGroups[c].vIndex.size( );
		fwrite(&meshGroups[c].vIndex[0], sizeof(DWORD), meshGroups[c].vIndex.size( ), fpW);//インデックス配列
	}

	//締めはバーテックスですよ
	char  vcTitle[16] = "POSITION";
	fwrite(vcTitle, sizeof(vcTitle), 1, fpW);//ヘッダ名
	DWORD  vc = vCoord2.size( );
	fwrite(&vc, sizeof(vc), 1, fpW);//件数
	fwrite(&vCoord2[0], sizeof(ML::Vec3), vCoord2.size( ), fpW);//データ


	char  ncTitle[16] = "NORMAL";
	fwrite(ncTitle, sizeof(ncTitle), 1, fpW);//ヘッダ名
	DWORD  nc = vNormal2.size( );
	fwrite(&nc, sizeof(nc), 1, fpW);//件数
	fwrite(&vNormal2[0], sizeof(ML::Vec3), vNormal2.size( ), fpW);//データ


	char  tcTitle[16] = "TEX_UV";
	fwrite(tcTitle, sizeof(tcTitle), 1, fpW);//ヘッダ名
	DWORD  tc = vUV2.size( );
	fwrite(&tc, sizeof(tc), 1, fpW);//件数
	fwrite(&vUV2[0], sizeof(ML::Vec2), vUV2.size( ), fpW);//データ

	fclose(fpW);	
	return true;
}
/*

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
float PAI = 3.141592653589;
#define kMAX_PATTERN 256
struct sAnimationPattern{		int frame;		RECT src;			RECT draw;			};
struct sScalePattern{			int frame;		float startValue;	float endValue;		};
struct sRotationPattern{		int frame;		float startValue;	float endValue;		};
struct sAlphaPattern{			int frame;		BYTE startValue;	BYTE endValue;		};
struct sMovePattern{			int frame;		POINT startPos;		POINT endPos;		};

sAnimationPattern	animp[kMAX_PATTERN];		int animCnt;		BOOL animEndFlag;
sScalePattern		scalep[kMAX_PATTERN];		int scaleCnt;		BOOL scaleEndFlag;
sRotationPattern	rotp[kMAX_PATTERN];			int rotCnt;			BOOL rotEndFlag;
sAlphaPattern		alphap[kMAX_PATTERN];		int alphaCnt;		BOOL alphaEndFlag;
sMovePattern		movep[kMAX_PATTERN];		int moveCnt;		BOOL moveEndFlag;
char				textureFileName[_MAX_PATH];

bool comvertAnimationData(char *data);
bool comvertRotationData(char *data);
bool comvertScalingData(char *data);
bool comvertAlphaData(char *data);
bool comvertMoveData(char *data);
void outputRep( );
bool readAnimationTextFile(char *inFileName);
bool writeAnimationFile(char *inFileName);



void main( int argc, char *argv[ ])
{
	int rtv;
	//ファイル名を取得
	if(argc < 2){
		printf("変換するファイルが指定されていません\n");
		getch( );
		return;
	}
	//変換対象のファイル名を表示
	char fileName[_MAX_PATH];
	strcpy(fileName, argv[1]);
	printf("以下のファイルを変換します。\n");
	printf("%s\n",fileName);

	char fileName2[_MAX_PATH];
	strcpy(fileName2, argv[1]);
	int len = (int)strlen(fileName2);
	strcpy(&fileName2[len - 3], "SAD");
	//変換後のファイル名を表示
	printf("以下のファイルを作成します。\n");
	printf("%s\n",fileName2);
	//変換処理実行の確認
	while(true)
	{
		printf("ファイルの変換を開始してもよろしいですか？(Y/N)\n");
		rtv = getch( );
		if(rtv == 'Y' || rtv == 'y'){ break;}
		if(rtv == 'N' || rtv == 'n'){
			printf("変換処理を中止しました\n");
			getch( );
			return;
		}
	}
	//ファイル読み込み開始
	if(false == readAnimationTextFile(fileName)){
		printf("読み込み処理中にエラーが起こりました\n");
		getch( );
		return;
	}
	//ファイル読み込み結果のレポート
	outputRep( );
	//変換ファイルの保存開始
	writeAnimationFile(fileName2);
	printf("変換処理が終了しました\n");
	getch( );
}
bool writeAnimationFile(char *inFileName)
{
	int cnt, fileLength = 0;
	//ファイルを開く
	FILE *fp;
	fp = fopen(inFileName, "w");
	if(fp == NULL){	return false;}
	int len, temp;
	
	fseek(fp, 8,SEEK_SET);
	//データを保存する
	if(textureFileName[0] != '\0'){
		fwrite("TEX ", 4, 1, fp);
		len = (int)strlen(textureFileName);
		temp = 1;
		fwrite(&temp, sizeof(int), 1, fp);
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(textureFileName, sizeof(char) * len, 1, fp);
		fileLength += len + 12;
	}
	if(animCnt > 0){
		fwrite("ANIM", 4, 1, fp);
		fwrite(&animCnt, sizeof(int), 1, fp);
		len = animCnt * sizeof(sAnimationPattern);
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(animp, len, 1, fp);
		fileLength += len + 12;
	}
	if(rotCnt > 0){
		fwrite("ROT ", 4, 1, fp);
		fwrite(&rotCnt, sizeof(int), 1, fp);
		len = rotCnt * sizeof(sRotationPattern);
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(rotp, len, 1, fp);
		fileLength += len + 12;
	}
	if(scaleCnt > 0){
		fwrite("SCAL", 4, 1, fp);
		fwrite(&scaleCnt, sizeof(int), 1, fp);
		len = scaleCnt * sizeof(sScalePattern);
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(scalep, len, 1, fp);
		fileLength += len + 12;
	}
	if(alphaCnt > 0){
		fwrite("ALPA", 4, 1, fp);
		fwrite(&alphaCnt, sizeof(int), 1, fp);
//		len = alphaCnt * sizeof(sAlphaPattern);
//		fwrite(&len, sizeof(int), 1, fp);
//		fwrite(&alphap, len, 1, fp);
		//バウンディングボックス対策
		len = alphaCnt * (sizeof(int) + sizeof(BYTE) + sizeof(BYTE));
		fwrite(&len, sizeof(int), 1, fp);
		for(cnt = 0; cnt < alphaCnt; cnt++){
			fwrite(&alphap[cnt].frame, sizeof(int), 1, fp);
			fwrite(&alphap[cnt].startValue, sizeof(BYTE), 1, fp);
			fwrite(&alphap[cnt].endValue, sizeof(BYTE), 1, fp);
		}
		fileLength += len + 12;
	}
	if(moveCnt > 0){
		fwrite("MOVE", 4, 1, fp);
		fwrite(&moveCnt, sizeof(int), 1, fp);
		len = moveCnt * sizeof(sMovePattern);
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(movep, len, 1, fp);
		fileLength += len + 12;
	}
	//ファイルヘッダを書き込む
	fseek(fp, 0,SEEK_SET);
	fwrite("S_AM", 4, 1, fp);
	fwrite(&fileLength, sizeof(int), 1, fp);
}

void outputRep( )
{
	printf("-----------------------------------------\n");
	printf("テクスチャファイル名\n");
	printf("[%s]\n", textureFileName);
	printf("アニメーションパターン\n");
	printf("データ数[%d]\n", animCnt);
	if(animEndFlag == FALSE && animCnt != 0){	printf("!アニメーションパターンに終端がありません\n");	}
	printf("回転パターン\n");
	printf("データ数[%d]\n", rotCnt);
	if(rotEndFlag == FALSE && rotCnt != 0){		printf("!回転パターンに終端がありません\n");	}
	printf("拡大縮小パターン\n");
	printf("データ数[%d]\n", scaleCnt);
	if(scaleEndFlag == FALSE && scaleCnt != 0){	printf("!拡大縮小パターンに終端がありません\n");	}
	printf("透過パターン\n");
	printf("データ数[%d]\n", alphaCnt);
	if(alphaEndFlag == FALSE && alphaCnt != 0){	printf("!透過パターンに終端がありません\n");	}
	printf("移動パターン\n");
	printf("データ数[%d]\n", moveCnt);
	if(moveEndFlag == FALSE && moveCnt != 0){	printf("!移動パターンに終端がありません\n");	}
	printf("-----------------------------------------\n");

}

bool readAnimationTextFile(char *inFileName)
{
	char buffer[256], *p;
	int id;
	//ファイルを開く
	FILE *fp;
	fp = fopen(inFileName, "r");
	if(fp == NULL){	return false;}

	//							アニメーション　回転　拡大　アルファ　テクスチャ　移動
	char *dataHeaderNames[] = {"AM:", "RT:", "SC:", "AP:", "TX:", "MV:"};
	//ファイルの終端まで読み込む
	while(NULL != fgets(buffer, 256, fp))
	{
		p = strchr(buffer, '\n');
		if(p != NULL){ *p = '\0';}

		//データヘッダを確認する
		for(id = 0; id < 6; id++){
			if(0 == strncmp(buffer, dataHeaderNames[id], 3)){
				break;
			}
		}
		//データヘッダ毎に対応した読み込み処理を行う
		switch(id){
			case 0:		comvertAnimationData(&buffer[3]);		break;
			case 1:		comvertRotationData(&buffer[3]);		break;
			case 2:		comvertScalingData(&buffer[3]);			break;
			case 3:		comvertAlphaData(&buffer[3]);			break;
			case 4:		strcpy(textureFileName, &buffer[3]);	break;
			case 5:		comvertMoveData(&buffer[3]);			break;
			default:		break;
		}
	}
	fclose(fp);
	return true;
}

bool comvertMoveData(char *data)
{
	sscanf(data, "%d, %d,%d, %d,%d",
				&movep[moveCnt].frame,
				&movep[moveCnt].startPos.x,	&movep[moveCnt].startPos.y,
				&movep[moveCnt].endPos.x,	&movep[moveCnt].endPos.y);
	if(movep[moveCnt].frame < 0){	moveEndFlag = TRUE;}
	moveCnt++;
	return true;
}
bool comvertAnimationData(char *data)
{
	sscanf(data, "%d, %d,%d,%d,%d, %d,%d,%d,%d",
				&animp[animCnt].frame,
				&animp[animCnt].src.left,	&animp[animCnt].src.top,	&animp[animCnt].src.right,	&animp[animCnt].src.bottom,
				&animp[animCnt].draw.left,	&animp[animCnt].draw.top,	&animp[animCnt].draw.right,	&animp[animCnt].draw.bottom);
	if(animp[animCnt].frame < 0){	animEndFlag = TRUE;}
	animCnt++;
	return true;
}
bool comvertRotationData(char *data)
{
	double tempS = 0, tempE = 0;
	sscanf(data, "%d, %lf, %lf",
				&rotp[rotCnt].frame, &tempS, &tempE);
	rotp[rotCnt].startValue = (tempS / 180.0) * PAI,
	rotp[rotCnt].endValue   = (tempE / 180.0) * PAI;
	if(rotp[rotCnt].frame < 0){	rotEndFlag = TRUE;}
	rotCnt++;
	return true;
}
bool comvertScalingData(char *data)
{
	sscanf(data, "%d, %f, %f",
				&scalep[scaleCnt].frame,
				&scalep[scaleCnt].startValue,
				&scalep[scaleCnt].endValue);
	if(scalep[scaleCnt].frame < 0){	scaleEndFlag = TRUE;}
	scaleCnt++;
	return true;
}
bool comvertAlphaData(char *data)
{
	int tempS, tempE;
	sscanf(data, "%d, %d, %d",
				&alphap[alphaCnt].frame,
				&tempS,
				&tempE);
    alphap[alphaCnt].startValue = (BYTE)tempS;
	alphap[alphaCnt].endValue   = (BYTE)tempE;
	if(alphap[alphaCnt].frame < 0){	alphaEndFlag = TRUE;}
	alphaCnt++;
	return true;
}
















*/