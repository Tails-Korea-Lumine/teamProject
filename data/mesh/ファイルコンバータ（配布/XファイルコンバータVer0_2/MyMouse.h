#pragma once
#include <windows.h>

class Mouse{

	enum ButState{
		Non,		//	�}�E�X�{�^���͉�����Ă��Ȃ�
		Click,		//	�}�E�X�{�^���̓N���b�N���ꂽ
		On,			//	�}�E�X�{�^���͉����������Ă���
		Off,		//	�}�E�X�{�^���͗����ꂽ����ł���
	};
	POINT pos;
	bool wml;
	bool wmr;
	bool l;	//���{�^��
	bool r;	//�E�{�^��
	bool pr_l;	//���{�^��(�P�t���[���O�̏�ԁj
	bool pr_r;	//�E�{�^��(�P�t���[���O�̏�ԁj

public:
	Mouse( );
	~Mouse( );
	void Check( );
	ButState LBStatus( );
	ButState RBStatus( );
	POINT Pos( );

	void LBStatus(bool s);
	void RBStatus(bool s);
	void Pos(POINT& p);
};
