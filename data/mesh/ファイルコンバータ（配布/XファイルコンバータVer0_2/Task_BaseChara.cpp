//-----------------------------------------------------------------------------
//�S�L�����N�^�̃x�[�X�ƂȂ�N���X�i��)
//-----------------------------------------------------------------------------
#include "Task_BaseChara.h"
//-----------------------------------------------------------------------------
//����������
//�@�\�T�v�F�^�X�N�������ɂP����s�����i�f�ނȂǂ̏������s���j
//-----------------------------------------------------------------------------
BChara::BChara(tstring n_):BTask(n_)
,pos(0,0,0)
,dirY(0), dirX(0)
,jumpSpeed(0)
,footHit(false)
,footAjast(0)			//NEW
{
	D3DXMatrixIdentity(&matW);
	//���S�Ă̒ǉ��ϐ���0�Ȃ��������̏�Ԃɏ���������R�[�h��������
}
//-----------------------------------------------------------------------------
//�������
//�@�\�T�v�F�^�X�N�I�����ɂP����s�����i�f�ނȂǂ̉�����s���j
//-----------------------------------------------------------------------------
BChara::~BChara( )
{
}
//-----------------------------------------------------------------------------
//�`��p�����[�^�X�V
//�@�\�T�v�F�`��Ɋ֘A����p�����[�^���X�V����
//-----------------------------------------------------------------------------
void BChara::UpDate_DrawInfo( )
{
//	���[���h�ϊ��s����X�V����
	D3DXMATRIX	matT, matRY, matRX;
//	Y���ɑ΂����]�s����쐬����
	D3DXMatrixRotationY(&matRY, dirY);
	D3DXMatrixRotationX(&matRX, dirX);
//	���s�ړ��s����쐬����
	D3DXMatrixTranslation(&matT, pos.x, pos.y, pos.z);
//	�s��̍���
	matW = matRY * matRX * matT;
//	�t�s��̐����i���[���h�ϊ��̋t�s��j
	D3DXMatrixInverse(&matW_inv,  NULL,  &matW);
}
//-----------------------------------------------------------------------------
//���C�ƃ��b�V�����̃o�E���f�B���O�{�b�N�X�̐ڐG����
bool BChara::HitCheck_BBox_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_)
{
	//���C�����b�V���̃��[�J�����W�n�ɕϊ�
	ML::Vec3  rayPos_inv;
	D3DXVec3TransformCoord(&rayPos_inv,  &rp_,  &matW_inv);
	ML::Vec3  rayDir_inv;
	D3DXVec3TransformNormal(&rayDir_inv,  &rd_,  &matW_inv);

	//���C�ƃ��b�V���̂a�{�b�N�X�Ƃ̐ڐG����
	DG::cMesh::SP  mesh = Mesh( );
	if( mesh &&
		true == mesh->HitCheck_BBox_Ray(rayPos_inv, rayDir_inv)){
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
//	���C�ƃ��b�V���̐ڐG������s��
bool BChara::HitCheck_Mesh_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_,  float&  per1_)
{
	//���C�����b�V���̃��[�J�����W�n�ɕϊ�
	ML::Vec3  rayPos_inv;
	D3DXVec3TransformCoord(&rayPos_inv,  &rp_,  &matW_inv);
	ML::Vec3  rayDir_inv;
	D3DXVec3TransformNormal(&rayDir_inv,  &rd_,  &matW_inv);

	//���C�ƃ��b�V���̐ڐG����
	DG::cMesh::SP  mesh = Mesh( );
	if( mesh &&
		true == mesh->HitCheck_Mesh_Ray(rayPos_inv, rayDir_inv, per1_)){
		return true;
	}
	return false;
}


