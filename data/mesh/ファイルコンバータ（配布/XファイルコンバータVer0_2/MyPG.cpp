#include "MyPG.h"
#include "Task_Game.h"

MyGameEngine* ge;

MyGameEngine::MyGameEngine( )
:GameEngine(
//	�����
	960,							//	��ʃT�C�YX
	540,							//	��ʃT�C�YY
	1,								//	�\���{��
	false,							//	�t���X�N���[�����[�h
	_T("3D_GPG"),					//	�E�C���h�E�^�C�g��
	_T("�R�c�Q�[���v���O���~���O"))	//	�E�B���h�E�N���X��
{
}

//�Q�[���G���W���ɒǉ��������̂̏������ƊJ��
bool MyGameEngine::Init(HWND wnd_)
{
	wh = wnd_;
	//	���͑��u�̐ݒ�
	DI::sKey	k1[10] = {
		//�L�[�{�[�h�̃L�[��
		//��		�v���O������Ŏg�p����{�^����
		//��		��
		{DIK_LEFT,	DI::X_M	},	//��
		{DIK_RIGHT,	DI::X_P	},	//�E
		{DIK_UP,	DI::Y_M	},	//��
		{DIK_DOWN,	DI::Y_P	},	//��
		{DIK_Z,		DI::B00	},	//00�{�^��
		{DIK_X,		DI::B01	},	//01�{�^��
		{DIK_C,		DI::B02	},	//02�{�^��
		{DIK_V,		DI::B03	},	//03�{�^��
		{DIK_B,		DI::B04	},	//04�{�^��
		{DIK_N,		DI::B05	},	//05�{�^��
	};
	//�L�[�{�[�h���͂��󂯎��I�u�W�F�N�g�𐶐�
	in1 = DI::cInput::CreateKB(10, k1);

	//	�Q�c�`�掞�̃X�N���[����
	offset.x = 0;
	offset.y = 0;
	//	�R�c�`�掞�̃J�����i�f�t�H���g�j
	//	�J�����̐ݒ�
	camera = DG::cCamera::Create(
		D3DXVECTOR3( 500.0f,   0.0f,   400.0f),		//	�^�[�Q�b�g�ʒu
		D3DXVECTOR3( 400.0f, 1200.0f, -800.0f),		//	�J�����ʒu
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),				//	�J�����̏�����x�N�g��
		35.0f * D3DX_PI / 180.0f, 10.0f, 10000.0f,	//	����p�E���싗��
		(float)ge->screenWidth / (float)ge->screenHeight);		//	��ʔ䗦

	//	���C�g�̐ݒ�i�f�t�H���g�j
	D3DLIGHT9 wL;
	ZeroMemory(	&wL, sizeof(wL));
	wL.Type = D3DLIGHT_DIRECTIONAL;				//�f�B���N�V���i�����C�g���g�p
	ML::Vec3 vecDir = ML::Vec3(3.0f,  -10.0f, 7.0f);		//�Ǝ˕���
	D3DXVec3Normalize((ML::Vec3*)&wL.Direction, &vecDir);
	wL.Diffuse.r = 1.0f;
	wL.Diffuse.g = 1.0f;
	wL.Diffuse.b = 1.0f;
	wL.Specular.r = 1.0f;
	wL.Specular.g = 1.0f;
	wL.Specular.b = 1.0f;
	wL.Ambient.r = 0.2f;
	wL.Ambient.g = 0.2f;
	wL.Ambient.b = 0.2f;
	light = wL;

	//�������s�^�X�N����
	PushBack( Game::Create( ) );
	return true;
}
MyGameEngine::~MyGameEngine( )
{
//	���͑��u�̉��
	in1.reset( );
}
//�Q�[���G���W���ɒǉ��������̂̃X�e�b�v����
void MyGameEngine::Step( )
{
//	���͑��u�̍X�V
	in1->UpDate( );
	mouse.Check( );
}

//2D�`����̃f�t�H���g�p�����[�^�ݒ�
void MyGameEngine::Set2DDrawState( )
{
}
//3D�`����̃f�t�H���g�p�����[�^�ݒ�
void MyGameEngine::Set3DDrawState( )
{
	//���C�g�̐ݒ�
	dg->Device( )->SetRenderState(D3DRS_LIGHTING, TRUE);
	dg->Device( )->LightEnable(0, TRUE);
	dg->Device( )->SetLight(0, &light);
	//d3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);	//�X�y�L�����L��

	//�A���t�@�L��
	dg->Device( )->SetRenderState(	D3DRS_ALPHABLENDENABLE,	TRUE);
	//�]�����u�����h�W��(�`)
	dg->Device( )->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//�]����u�����h�W��(�P�|�`)
	dg->Device( )->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//�e�N�X�`������
	dg->Device( )->SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE ); 
	dg->Device( )->SetTextureStageState(0, D3DTSS_COLOROP,		D3DTOP_MODULATE ); 
	dg->Device( )->SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE ); 

	dg->Device( )->SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE ); 
	dg->Device( )->SetTextureStageState(0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE ); 
	dg->Device( )->SetTextureStageState(0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );

	//	�J�����̐ݒ�X�V
	camera->UpDate( );

	D3DVIEWPORT9 vp = {0,0,ge->screenWidth, ge->screenHeight, 0.0f, 1.0f};
	dg->Device( )->SetViewport(&vp);
}