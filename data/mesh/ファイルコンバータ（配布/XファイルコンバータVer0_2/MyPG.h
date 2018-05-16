#pragma once
#include "GameEngine_Ver1_1.h"
//#include "DI2012.h"
//#include "DG2014.h"
#include "MyMouse.h"
#include <fstream>
#include <sstream>

class MyGameEngine : public GameEngine
{
public:
	MyGameEngine( );
//�Q�[���G���W���ɒǉ��������̂̏������ƊJ��
	bool Init(HWND wnd_);
	~MyGameEngine( );
//�Q�[���G���W���ɒǉ��������̂̃X�e�b�v����
	void Step( );

//3DPG1�Ή��ɂ��ǉ�
	//2D�`����̃f�t�H���g�p�����[�^�ݒ�
	void Set2DDrawState( );
	//3D�`����̃f�t�H���g�p�����[�^�ݒ�
	void Set3DDrawState( );
//�Q�[���G���W���ɒǉ����������͉̂��ɉ�����
//----------------------------------------------
	Mouse			mouse;		//	�}�E�X
	DI::cInput::SP	in1;		//	���͋@��
	POINT			offset;		//	�Q�c�`�掞�̃X�N���[����

	DG::cCamera::SP camera;		//	�J����
	D3DLIGHT9		light;		//	���C�g

	HWND			wh;
	tstring			sobPath;
//----------------------------------------------
};

extern MyGameEngine* ge;
