#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//�S�L�����N�^�̃x�[�X�ƂȂ�N���X�i��)
//�S�L�����N�^�����ʂŎ��ϐ��͂����Őݒ肷��B
//�i�S�L�����N�^�łȂ��Ƃ��A�p�ɂɗ��p�����ϐ�������Ȃ炱���ɉ�����j
//-----------------------------------------------------------------------------
#include "MyPG.h"

class BChara : public BTask
{
public:
	BChara(tstring n_);
	virtual ~BChara( );

	typedef shared_ptr<BChara>	SP;	//�V�F�A�|�C���^����
	typedef weak_ptr<BChara>	WP;	//�E�B�[�N�|�C���^����
//-----------------------------------------------------------------------------
//�^�X�N���ϐ��̐錾�i�ʁj
//-----------------------------------------------------------------------------
	ML::Vec3	pos;		//�L�����N�^�̍��W
	float		footAjast;	//�������莞�̍��������l
	ML::Vec3	FootPos( ){ ML::Vec3 w = pos; w.y+=footAjast; return w;}

	float		jumpSpeed;	//�㏸���������x
	bool		footHit;	//���n���Ă���
	float		dirY, dirX;	//�L�����N�^�̌���
	ML::Mat4x4	matW;		//���[���h�ϊ��s��
	ML::Mat4x4	matW_inv;	//���[���h�ϊ��s��̋t�s��
//-----------------------------------------------------------------------------
//�ǉ��֐�
//-----------------------------------------------------------------------------
	void UpDate_DrawInfo( );	//�`��p�����[�^�X�V
	virtual  DG::cMesh::SP  Mesh( ) = 0;	//���b�V����Ԃ�
//	���C�ƃ��b�V���i�a�{�b�N�X�j�̐ڐG������s��
	bool HitCheck_BBox_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_);
//	���C�ƃ��b�V���̐ڐG������s��
	bool HitCheck_Mesh_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_,  float&  per1_);

};
