#pragma once
#pragma warning(disable:4996)
#include "GameEngine_Ver3_7.h"

//�Ւf�@�̃N���X

class Breaker
{
private:
	//�Ւf�@�̈ʒu
	ML::Vec3 pos; 
	//�����蔻��͈�
	ML::Box3D active_Base;
	//�I���I�t�̃t���O
	bool active_Falg;

public:
	//�v���C���Ƃ̂����蔻�� ���� : (�v���C���̂����蔻��͈�)
	bool Player_Touch_Breaker(const ML::Box3D& hit);
	//�u���[�J�[���N��
	void Activate_Breaker();
	//���̃u���[�J�[�̏�Ԃ����炤
	bool Get_Now_State();
	//�ʒu�擾
	ML::Vec3 Get_Pos();

	//�R���X�g���N�^�E�f�X�g���N�^
	//�[���N���A
	Breaker();
	//�ʒu�w��
	Breaker(ML::Vec3 pos);

	~Breaker() {}
};