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


struct sFace{	DWORD  v, t, n;};
struct sVertex{	D3DXVECTOR3  v, n;	D3DXVECTOR2 t;};


bool  ObjToSOB(char  fpathR_[], char  fpathW_[]); 	
//�t�@�C���`�F�b�N
bool OBJFileSub_FileCheck(FILE*  fp_, int&  usemtlNum_, string&  matFile_);
//OBJ�t�@�C���p�}�e���A���t�@�C������}�e���A����ǂݍ���
bool LoadMaterial_FromOBJFile(const tstring&		fpath_);
//�f�t�H���g�}�e���A���R���e�i�ƃ��b�V���O���[�v�����i�}�e���A���̖����t�@�C���j
void OBJFileSub_DefMaterialContainer_MeshGroup( );
//OBJ�t�@�C�����璸�_����ǂݏo��
void OBJFileSub_ReadVertex(FILE*  fp_, 
	vector<D3DXVECTOR3>& v_, vector<D3DXVECTOR3>& n_, vector<D3DXVECTOR2>& t_);
//OBJ�t�@�C������t�F�C�X����ǂݏo��
void OBJFileSub_ReadFace(FILE*  fp_, vector<pair<tstring, vector<sFace>>>& f_);
//�t�F�C�X�W�J
void OBJFileSub_FaceVertex( vector<pair<tstring, vector<sFace>>>& f_, 
							vector<D3DXVECTOR3>& v_, vector<D3DXVECTOR3>& n_, vector<D3DXVECTOR2>& t_,
							vector<sVertex>& vertex_);
//�d�����钸�_����菜��
void OBJFileSub_VertexRemove(	vector<sVertex>& base_, 	vector<sVertex>& out_);
//�S�̂ɑ΂���C���f�b�N�X�𐶐�
void OBJFileSub_CreateIndexAll(vector<DWORD>& index_, vector<sVertex>& base_, vector<sVertex>& ex_);

void main( int argc, char *argv[ ])
{
	int rtv;
	printf("OBJ�`���t�@�C����ϊ�����BVer0.1\n\n");
	//�t�@�C�������擾
	if(argc < 2){
		printf("�ϊ�����t�@�C�����w�肳��Ă��܂���\n");
		getch( );
		return;
	}
	//�ϊ��Ώۂ̃t�@�C������\��
	char fileName[_MAX_PATH];
	strcpy(fileName, argv[1]);
	printf("�ȉ��̃t�@�C����ϊ����܂��B\n");
	printf("%s\n",fileName);

	char fileName2[_MAX_PATH];
	strcpy(fileName2, argv[1]);
	int len = (int)strlen(fileName2);
	strcpy(&fileName2[len - 3], "SOB");
	//�ϊ���̃t�@�C������\��
	printf("�ȉ��̃t�@�C�����쐬���܂��B\n");
	printf("%s\n",fileName2);
	//�ϊ��������s�̊m�F
	while(true)
	{
		printf("�t�@�C���̕ϊ����J�n���Ă���낵���ł����H(Y/N)\n");
		rtv = getch( );
		if(rtv == 'Y' || rtv == 'y'){ break;}
		if(rtv == 'N' || rtv == 'n'){
			printf("�ϊ������𒆎~���܂���\n");
			getch( );
			return;
		}
	}
	if(false == ObjToSOB(fileName, fileName2)){
		printf("�ǂݍ��ݏ������ɃG���[���N����܂���\n");
		_getch( );
		return;
	}
	//�t�@�C���ǂݍ��݌��ʂ̃��|�[�g
	printf("�ϊ��������I�����܂���\n");
	_getch( );
}

//�t�@�C���`�F�b�N
bool OBJFileSub_FileCheck(FILE*  fp_, int&  usemtlNum_, string&  matFile_)
{
	//�}�e���A���̗L���y�уt�F�C�X�O���[�v�������o
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
	//�}�e���A���O���[�v��������
	if(usemtlCnt == 0){
		ML::MsgBox(_T("usemtl������܂���"));
	}
	if(matFile == false){
		ML::MsgBox(_T("mtllib������܂���"));
		return false;
	}
	return true;
}
//OBJ�t�@�C���p�}�e���A���t�@�C������}�e���A����ǂݍ���
bool LoadMaterial_FromOBJFile(const tstring&		fpath_)
{
	//�t�@�C���p�X���}���`�o�C�g�ɕϊ�
	string  fn = ML::ToString(fpath_);
	//�t�@�C�����J���ē��e��ǂݍ���
	FILE* fp=NULL;
	fopen_s(&fp, fn.c_str( ),"rt");


	char data[256]={0};
	
	vector<cMaterialContainerS>::iterator  it;
	D3DXVECTOR4 v(0,0,0,0);
	while(fgets(data, 256, fp)){
		//�}�e���A����
		if(strncmp(data,"newmtl ", 7)==0){
			//�}�e���A�������o�Ă�����̈��V��
			cMaterialContainerS  w={0};
			w.diffuse  = ML::Color(1,1,1,1);	//�}�e���A�����:�f�t���[�Y
			w.ambient  = ML::Color(1,1,1,1);	//�}�e���A�����:�A���r�G���g
			w.speculer = ML::Color(0,0,0,0);	//�}�e���A�����:�X�y�L����
			w.emissive = ML::Color(0,0,0,0);	//�}�e���A�����:�G�~�b�V�u
			mateConts.push_back(w);
			it = mateConts.end( );
			it--;
			//���̂�o�^
			sscanf_s(&data[7],"%s ",it->name,sizeof(it->name));
		}
		//Ka�@�A���r�G���g
		else
		if(strncmp(data,"Ka ", 3)==0){
			sscanf_s(&data[3],"%f %f %f",&v.x,&v.y,&v.z);
			it->ambient = ML::Color(1,v.x,v.y,v.z);
		}
		//Kd�@�f�B�t���[�Y
		else
		if(strncmp(data,"Kd ", 3)==0){
			sscanf_s(&data[3],"%f %f %f",&v.x,&v.y,&v.z);
			it->diffuse = ML::Color(1,v.x,v.y,v.z);
		}
		//Ks�@�X�y�L�����[
		else
		if(strncmp(data,"Ks ", 3)==0){
			sscanf_s(&data[3],"%f %f %f",&v.x,&v.y,&v.z);
			it->speculer = ML::Color(1,v.x,v.y,v.z);
		}
		//map_Kd�@�e�N�X�`���t�@�C����
		else
		if(strncmp(data,"map_Kd ", 7)==0){
			sscanf_s(&data[7],"%s ",it->texName,sizeof(it->texName));
		}

	}
	fclose(fp);
	return true;
}
//�f�t�H���g�}�e���A���R���e�i�ƃ��b�V���O���[�v�����i�}�e���A���̖����t�@�C���j
void OBJFileSub_DefMaterialContainer_MeshGroup( )
{
	cMaterialContainerS  w = {0};
	w.diffuse  = ML::Color(1,1,1,1);	//�}�e���A�����:�f�t���[�Y
	w.ambient  = ML::Color(1,1,1,1);	//�}�e���A�����:�A���r�G���g
	w.speculer = ML::Color(0,0,0,0);	//�}�e���A�����:�X�y�L����
	mateConts.push_back(w);

	cMeshGroupS  wmg = {0};
	meshGroups.push_back(wmg);
}
//OBJ�t�@�C�����璸�_����ǂݏo��
void OBJFileSub_ReadVertex(FILE*  fp_, 
	vector<D3DXVECTOR3>& v_, vector<D3DXVECTOR3>& n_, vector<D3DXVECTOR2>& t_)
{
	//�O���ڂ̓_�~�[�f�[�^
	v_.push_back( D3DXVECTOR3(0,0,0) );
	n_.push_back( D3DXVECTOR3(0,0,0) );
	t_.push_back( D3DXVECTOR2(0,0) );
	//�}�e���A���̗L���y�уt�F�C�X�O���[�v�������o
	fseek(fp_,SEEK_SET,0);
	char data[256];
	while(fgets(data, 256, fp_)){
		//���_
		if(strncmp(data,"v ", 2)==0){
			D3DXVECTOR3  w;
			sscanf_s(&data[2], "%f %f %f", &w.x, &w.y, &w.z);
			v_.push_back(w);
		}
		//�@��
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
//OBJ�t�@�C������t�F�C�X����ǂݏo��
void OBJFileSub_ReadFace(FILE*  fp_, vector<pair<tstring, vector<sFace>>>& f_)
{
	//�}�e���A���̗L���y�уt�F�C�X�O���[�v�������o
	fseek(fp_,SEEK_SET,0);
	char data[256];
	int mode=-1;
	//�f�[�^�̍\�����`�F�b�N
	while(fgets(data, 256, fp_)){
		if(strncmp(data,"f ", 2)==0){
			sFace  wf[3] = {0};
			//���_/UV/�@��
			if(		9 == sscanf_s(&data[2], "%d/%d/%d %d/%d/%d %d/%d/%d ",	&wf[0].v,&wf[0].t,&wf[0].n,	&wf[1].v,&wf[1].t,&wf[1].n,	&wf[2].v,&wf[2].t,&wf[2].n)){	mode=0; break;}
			//���_//�@��
			else if(6 == sscanf_s(&data[2], "%d//%d %d//%d %d//%d ",		&wf[0].v,         &wf[0].n,	&wf[1].v,         &wf[1].n,	&wf[2].v,         &wf[2].n)){	mode=1; break;}
			//���_/UV/
			else if(6 == sscanf_s(&data[2], "%d/%d/ %d/%d/ %d/%d/ ",		&wf[0].v,&wf[0].t,			&wf[1].v,&wf[1].t,			&wf[2].v,&wf[2].t)){			mode=2; break;}
			//���_//
			else if(3 == sscanf_s(&data[2], "%d// %d// %d// ",				&wf[0].v,					&wf[1].v,					&wf[2].v)){						mode=3; break;}
		}
	}
	if(mode==-1){return;}

	//�ǂݍ���
	fseek(fp_,SEEK_SET,0);
	vector<pair<tstring, vector<sFace>>>::iterator  it;
	while(fgets(data, 256, fp_)){
		if(strncmp(data,"f ", 2)==0){
			sFace  wf[3] = {0};
			if(mode == 0){			//���_/UV/�@��
				sscanf_s(&data[2], "%d/%d/%d %d/%d/%d %d/%d/%d ",	&wf[0].v,&wf[0].t,&wf[0].n,	&wf[1].v,&wf[1].t,&wf[1].n,	&wf[2].v,&wf[2].t,&wf[2].n);
			}else if(mode == 1){	//���_//�@��
				sscanf_s(&data[2], "%d//%d %d//%d %d//%d ",			&wf[0].v,         &wf[0].n,	&wf[1].v,         &wf[1].n,	&wf[2].v,         &wf[2].n);
			}else if(mode == 2){	//���_/UV/
				sscanf_s(&data[2], "%d/%d/ %d/%d/ %d/%d/ ",			&wf[0].v,&wf[0].t,			&wf[1].v,&wf[1].t,			&wf[2].v,&wf[2].t);
			}else if(mode == 3){	//���_//
				sscanf_s(&data[2], "%d// %d// %d// ",				&wf[0].v,					&wf[1].v,					&wf[2].v);
			}
			for(int c=0; c<3; c++){
				it->second.push_back(wf[c]);
			}
		}
		else
		if(strncmp(data,"usemtl ", 7)==0){
			//�}�e���A�������o�Ă�����̈��V��
			f_.resize( f_.size( ) + 1);
			it  =  f_.end( );
			it--;
			//���̂�o�^
			char n[256]={0};
			sscanf_s(&data[7],"%s ",n,sizeof(n));
			it->first = ML::ToTstring(n);
		}
	}
}

//�t�F�C�X�W�J
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
//�d�����钸�_����菜��
void OBJFileSub_VertexRemove(	vector<sVertex>& base_, 	vector<sVertex>& out_)
{
	for(DWORD i=0; i<base_.size(); i++){
		bool f=true;
		for(DWORD j=0; j<out_.size(); j++){
			//���ɓ������̂�����Ƃ�
			if(	base_[i].v == out_[j].v &&
				base_[i].n == out_[j].n &&
				base_[i].t == out_[j].t ){
				f=false;
			}
		}
		//�d�����Ȃ���Γo�^
		if(f){
			out_.push_back(base_[i]);
		}
	}
}
//�S�̂ɑ΂���C���f�b�N�X�𐶐�
void OBJFileSub_CreateIndexAll(vector<DWORD>& index_, vector<sVertex>& base_, vector<sVertex>& ex_)
{
	index_.resize(base_.size());
	for(DWORD i=0; i<index_.size(); i++){
		bool f=true;
		for(DWORD j=0; j<ex_.size(); j++){
			//�������̂���������
			if(	base_[i].v == ex_[j].v &&
				base_[i].n == ex_[j].n &&
				base_[i].t == ex_[j].t ){
				index_[i] = j;	//�C���f�b�N�X�o�^
				break;
			}
		}
	}
}






//OBJ�t�@�C����ǂݍ��݁A�ʌ`���ŕۑ�����
bool  ObjToSOB(char  fpathR_[], char  fpathW_[])
{
	//�ǂݍ��ݑ��̃t�@�C�����J��
	FILE  *fpR;
	char data[256]={0};
	//�t�@�C�����J���ē��e��ǂݍ���
	fopen_s(&fpR, fpathR_,"rt");

	//�t�@�C���`�F�b�N
	int  usemtlNum;
	string  matFileName;
	if( OBJFileSub_FileCheck(fpR, usemtlNum, matFileName) ){
		//�}�e���A�����L��ꍇ�A�}�e���A����ǂݍ���
		tstring  mfp = ML::CreateFilePath(ML::ToTstring(fpathR_),ML::ToTstring(matFileName.c_str( )));
		LoadMaterial_FromOBJFile(mfp);
	}else{
		//�}�e���A���������ꍇ�A�}�e���A�������
		OBJFileSub_DefMaterialContainer_MeshGroup( );
	}

	//���_���̓ǂݍ���
	vector<D3DXVECTOR3>		vCoord;
	vector<D3DXVECTOR3>		vNormal;
	vector<D3DXVECTOR2>		vUV;
	OBJFileSub_ReadVertex(fpR, vCoord,  vNormal,  vUV);
	
	//�t�F�C�X��ǂݍ���
	vector<pair<tstring, vector<sFace>>>	vFace;
	OBJFileSub_ReadFace(fpR, vFace);

	//�t�@�C������K�v�ȃf�[�^�͂��ׂēǂݍ��񂾂�
	fclose(fpR);	

	//�t�F�C�X�W�J
	vector<sVertex>			vVertex;
	OBJFileSub_FaceVertex(vFace, vCoord, vNormal, vUV, vVertex);

	//�d�����钸�_���������������_�z��𐶐�
	vector<sVertex>			vVertex2;
	OBJFileSub_VertexRemove(vVertex, vVertex2);

	//�S�̂ɑ΂���C���f�b�N�X�𐶐�
	vector<DWORD>	vIndex;
	OBJFileSub_CreateIndexAll(vIndex, vVertex, vVertex2);

	//�e�o�b�t�@�ɕ���
	vector<D3DXVECTOR3>		vCoord2(vVertex2.size());
	vector<D3DXVECTOR3>		vNormal2(vVertex2.size());
	vector<D3DXVECTOR2>		vUV2(vVertex2.size());
	for(DWORD i=0; i<vVertex2.size(); i++){
		vCoord2[i] =	vVertex2[i].v;
		vNormal2[i] =	vVertex2[i].n;
		vUV2[i] =		vVertex2[i].t;
	}
	//�C���f�b�N�X���e�O���[�v�ɕ���
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

	//���b�V���O���[�v����
	for(auto  it = vFace2.begin( ); it != vFace2.end( ); it++){
		cMeshGroupS wmg;
		//���O�̈�v����}�e���A����T��
		for(DWORD c2 = 0; c2 < mateConts.size( );  c2++){
			if(it->first == ML::ToTstring(mateConts[c2].name)){
				wmg.mcNumber = c2;
			}
		}
		//�C���f�b�N�X����������
		wmg.indexNum = it->second.size( );
		wmg.vIndex.resize(wmg.indexNum);
		for(DWORD  c3 = 0;  c3 != wmg.vIndex.size( );  c3++){
			wmg.vIndex[c3] = it->second[c3];
		}
		//�}�e���A���O���[�v�ɐݒ�
		meshGroups.push_back(wmg);
	}


	//�t�@�C���ɏo�͂��܂���`
	FILE *fpW;
	//
	fopen_s(&fpW, fpathW_,"wb");

	//�t�@�C���w�b�_
	sFileHeader  kind = {kFileHeader};
	fwrite(&kind, sizeof(kind), 1, fpW);

	//�}�e���A���R���e�i�����܂���
	DWORD  mc = mateConts.size( );
	fwrite(&mc, sizeof(mc), 1, fpW);
	fwrite(&mateConts[0], sizeof(cMaterialContainerS), mateConts.size( ), fpW);

	//���b�V���O���[�v�����܂���
	DWORD  mg = meshGroups.size( );
	fwrite(&mg, sizeof(mg), 1, fpW);//����
	for(DWORD  c = 0; c < meshGroups.size( ); c++){
		fwrite(&meshGroups[c].mcNumber, sizeof(DWORD), 1, fpW);//�Ή��}�e���A���ԍ�
		fwrite(&meshGroups[c].indexNum, sizeof(DWORD), 1, fpW);//�C���f�b�N�X��
		DWORD tm =  meshGroups[c].vIndex.size( );
		fwrite(&meshGroups[c].vIndex[0], sizeof(DWORD), meshGroups[c].vIndex.size( ), fpW);//�C���f�b�N�X�z��
	}

	//���߂̓o�[�e�b�N�X�ł���
	char  vcTitle[16] = "POSITION";
	fwrite(vcTitle, sizeof(vcTitle), 1, fpW);//�w�b�_��
	DWORD  vc = vCoord2.size( );
	fwrite(&vc, sizeof(vc), 1, fpW);//����
	fwrite(&vCoord2[0], sizeof(ML::Vec3), vCoord2.size( ), fpW);//�f�[�^


	char  ncTitle[16] = "NORMAL";
	fwrite(ncTitle, sizeof(ncTitle), 1, fpW);//�w�b�_��
	DWORD  nc = vNormal2.size( );
	fwrite(&nc, sizeof(nc), 1, fpW);//����
	fwrite(&vNormal2[0], sizeof(ML::Vec3), vNormal2.size( ), fpW);//�f�[�^


	char  tcTitle[16] = "TEX_UV";
	fwrite(tcTitle, sizeof(tcTitle), 1, fpW);//�w�b�_��
	DWORD  tc = vUV2.size( );
	fwrite(&tc, sizeof(tc), 1, fpW);//����
	fwrite(&vUV2[0], sizeof(ML::Vec2), vUV2.size( ), fpW);//�f�[�^

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
	//�t�@�C�������擾
	if(argc < 2){
		printf("�ϊ�����t�@�C�����w�肳��Ă��܂���\n");
		getch( );
		return;
	}
	//�ϊ��Ώۂ̃t�@�C������\��
	char fileName[_MAX_PATH];
	strcpy(fileName, argv[1]);
	printf("�ȉ��̃t�@�C����ϊ����܂��B\n");
	printf("%s\n",fileName);

	char fileName2[_MAX_PATH];
	strcpy(fileName2, argv[1]);
	int len = (int)strlen(fileName2);
	strcpy(&fileName2[len - 3], "SAD");
	//�ϊ���̃t�@�C������\��
	printf("�ȉ��̃t�@�C�����쐬���܂��B\n");
	printf("%s\n",fileName2);
	//�ϊ��������s�̊m�F
	while(true)
	{
		printf("�t�@�C���̕ϊ����J�n���Ă���낵���ł����H(Y/N)\n");
		rtv = getch( );
		if(rtv == 'Y' || rtv == 'y'){ break;}
		if(rtv == 'N' || rtv == 'n'){
			printf("�ϊ������𒆎~���܂���\n");
			getch( );
			return;
		}
	}
	//�t�@�C���ǂݍ��݊J�n
	if(false == readAnimationTextFile(fileName)){
		printf("�ǂݍ��ݏ������ɃG���[���N����܂���\n");
		getch( );
		return;
	}
	//�t�@�C���ǂݍ��݌��ʂ̃��|�[�g
	outputRep( );
	//�ϊ��t�@�C���̕ۑ��J�n
	writeAnimationFile(fileName2);
	printf("�ϊ��������I�����܂���\n");
	getch( );
}
bool writeAnimationFile(char *inFileName)
{
	int cnt, fileLength = 0;
	//�t�@�C�����J��
	FILE *fp;
	fp = fopen(inFileName, "w");
	if(fp == NULL){	return false;}
	int len, temp;
	
	fseek(fp, 8,SEEK_SET);
	//�f�[�^��ۑ�����
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
		//�o�E���f�B���O�{�b�N�X�΍�
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
	//�t�@�C���w�b�_����������
	fseek(fp, 0,SEEK_SET);
	fwrite("S_AM", 4, 1, fp);
	fwrite(&fileLength, sizeof(int), 1, fp);
}

void outputRep( )
{
	printf("-----------------------------------------\n");
	printf("�e�N�X�`���t�@�C����\n");
	printf("[%s]\n", textureFileName);
	printf("�A�j���[�V�����p�^�[��\n");
	printf("�f�[�^��[%d]\n", animCnt);
	if(animEndFlag == FALSE && animCnt != 0){	printf("!�A�j���[�V�����p�^�[���ɏI�[������܂���\n");	}
	printf("��]�p�^�[��\n");
	printf("�f�[�^��[%d]\n", rotCnt);
	if(rotEndFlag == FALSE && rotCnt != 0){		printf("!��]�p�^�[���ɏI�[������܂���\n");	}
	printf("�g��k���p�^�[��\n");
	printf("�f�[�^��[%d]\n", scaleCnt);
	if(scaleEndFlag == FALSE && scaleCnt != 0){	printf("!�g��k���p�^�[���ɏI�[������܂���\n");	}
	printf("���߃p�^�[��\n");
	printf("�f�[�^��[%d]\n", alphaCnt);
	if(alphaEndFlag == FALSE && alphaCnt != 0){	printf("!���߃p�^�[���ɏI�[������܂���\n");	}
	printf("�ړ��p�^�[��\n");
	printf("�f�[�^��[%d]\n", moveCnt);
	if(moveEndFlag == FALSE && moveCnt != 0){	printf("!�ړ��p�^�[���ɏI�[������܂���\n");	}
	printf("-----------------------------------------\n");

}

bool readAnimationTextFile(char *inFileName)
{
	char buffer[256], *p;
	int id;
	//�t�@�C�����J��
	FILE *fp;
	fp = fopen(inFileName, "r");
	if(fp == NULL){	return false;}

	//							�A�j���[�V�����@��]�@�g��@�A���t�@�@�e�N�X�`���@�ړ�
	char *dataHeaderNames[] = {"AM:", "RT:", "SC:", "AP:", "TX:", "MV:"};
	//�t�@�C���̏I�[�܂œǂݍ���
	while(NULL != fgets(buffer, 256, fp))
	{
		p = strchr(buffer, '\n');
		if(p != NULL){ *p = '\0';}

		//�f�[�^�w�b�_���m�F����
		for(id = 0; id < 6; id++){
			if(0 == strncmp(buffer, dataHeaderNames[id], 3)){
				break;
			}
		}
		//�f�[�^�w�b�_���ɑΉ������ǂݍ��ݏ������s��
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