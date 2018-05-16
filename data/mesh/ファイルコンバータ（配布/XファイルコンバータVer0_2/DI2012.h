#pragma warning(disable:4996)
#pragma once
#include <windows.h>
#define DIRECTINPUT_VERSION	0x0800
#pragma comment(lib,"dxguid")
#pragma comment(lib,"dInput8")
#include <dInput.h>
#include "myLib.h"
#include <string>
#include <vector>

//����������������������������������������������������������������������������������������������
//���摜����n�@�\�Q																		�@��
//����������������������������������������������������������������������������������������������
namespace DI
{
	//��O�Ƃ��ē�������G���[���
	enum eErr
	{
		ERR_NOTHING,		//�G���[�Ȃ�
		ERR_CREATE,			//�������s
		ERR_NOTDEVICE,		//�f�o�C�X���擾�I�Ȃ�
		ERR_UNNONTYPE,		//�s���ȃf�o�C�X���
		ERR_ENUMDEVICE,		//�f�o�C�X�̗񋓂Ɏ��s
		ERR_CREATEDEVICE,	//�f�o�C�X�̐������s
		ERR_DATAFORMAT,		//�f�[�^�t�H�[�}�b�g�̐ݒ莸�s
		ERR_COOPLEVEL,		//�������x���ݒ莸�s
		ERR_GPAXIS,			//
		ERR_KBPROPERTY,
	};
	enum KS{	//�L�[���	
		Err, 
		On,		//�����Ă���Ԓ�
		Off,	//�����Ă���Ԓ�
		Down,	//����������i�P�t���[���̂ݔ����j
		Up,		//����������i�P�t���[���̂ݔ����j
	};
	enum KK{	//�v���O������Ŏg�p�ł���L�[�̎��
		X_M, X_P, Y_M, Y_P,	//	�S�����L�[
		B00, B01, B02, B03, B04, B05, B06, B07,
		B08, B09, B10, B11, B12, B13, B14, B15,
		KMax		//�@�{�^������
	};

	namespace GPK	//�Q�[���p�b�h�̃L�[��\��
	{
		enum Type{		B00, B01, B02, B03, B04, B05, B06, B07,
						B08, B09, B10, B11, B12, B13, B14, B15,
						Y_M, Y_P, X_M, X_P,	//	�S�����L�[
						KMax		//�@�{�^������
		};
	}

	class cObj
	{
		LPDIRECTINPUT8			di;
		HWND					wnd;

		typedef weak_ptr<cObj> WP;
		static cObj::WP		winst;		//	�P�������݂ł��Ȃ��C���X�^���X�̃E�B�[�N�|�C���^
//		cObj( ){ }
		cObj(HWND wnd_);	
	public:
		typedef shared_ptr<cObj> SP;
		~cObj( );
		//	DI���C�u�����𐶐�����
		static cObj::SP Create(HWND wnd_);
		static cObj::SP GetInst( );
		HWND Wnd( ){	return wnd;}
		LPDIRECTINPUT8 Object( ){	return di;}
	};

	struct sKey{
		DWORD	vKind;		//���ۂ̃{�^���܂��̓L�[�{�[�h�̃o�[�`�����L�[
		KK		kKind;		//�{�^���̎�ށi�o�f��ł̖��́j
	};
	class cInput
	{
		enum DeviceKind{	KeyBoard, GamePad, UnNon	};	//�f�o�C�X�̎��
		struct sKeyParam{
			DWORD		vKind;		//���ۂ̃{�^���܂��̓L�[�{�[�h�̃o�[�`�����L�[
			KK			kKind;		//�{�^���̎�ށi�o�f��ł̖��́j
			DWORD		kState;		//�L�[�̏��
		};
		IDirectInputDevice8*	device;
		DeviceKind				dk;
		vector<sKeyParam>		kp;

		cInput( ){ }
		cInput(	DeviceKind			dk_,
				DWORD				id_,
				DWORD				numKey_,
				sKey				kpa_[ ]);
		eErr CreateDevice(DWORD id_);

		void CheckKB( );
		void CheckGP( );
	public:
		~cInput( );
		typedef shared_ptr<cInput> SP;
		typedef weak_ptr<cInput> WP;
	//	�f�o�C�X�𐶐�����
		static SP CreateKB(	DWORD				numKey_,
							sKey				kpa_[ ]);
		static SP CreateGP(	DWORD				id_,
							DWORD				numKey_,
							sKey				kpa_[ ]);
	//	�X�V
		void UpDate( );
	//	�{�^���̏�Ԃ��m�F
		bool Check( KK		kk_,
					KS		ks_);	//���o����{�^���̏��
	private:
		static SP Create(	DeviceKind			dk_,
							DWORD				id_,
							DWORD				numKey_,
							sKey				kpa_[ ]);
		SP link;
	public:
		//�Q�̃f�o�C�X�𓝍�����
		//���S�����l����Ȃ�A�N���X���\�b�h��Link���g����
		SP Link(SP l){ 
			if(this == l.get( )){	return 0;	}
			else{					SP w =link; link = l; return link; }
		}
		//�Q�̃f�o�C�X�𓝍�����
		//�Е��̃f�o�C�X�������ł��Ă��Ȃ��ꍇ�̑Ή�����
		static cInput::SP Link(SP a, SP b);
	};
}

