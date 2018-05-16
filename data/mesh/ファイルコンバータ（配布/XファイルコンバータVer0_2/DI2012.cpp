#include "DI2012.h"


namespace DI
{
	cObj::WP cObj::winst;	//	�P�������݂ł��Ȃ��C���X�^���X

	cObj::cObj(HWND wnd_)
	{
		wnd = wnd_;
		if(FAILED(DirectInput8Create(	GetModuleHandle(NULL),
										DIRECTINPUT_VERSION,
										IID_IDirectInput8,
										(VOID**)&di,
										NULL)))
		{
			//	�f�o�C�X�������s
			throw ERR_CREATE;
		}
	}
	cObj::~cObj( )
	{
	//	DI�I�u�W�F�N�g�̉��
		ML::SafeRelease(&di);
	}
	//	DI���C�u�����𐶐�����
	cObj::SP cObj::Create(	HWND		hw_)	//	�E�B���h�E�n���h��
	{
		if(winst.expired( )){
			cObj::SP sp = cObj::SP(new cObj(hw_));
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

	cInput::cInput(	DeviceKind			dk_,
					DWORD				id_,
					DWORD				numKey_,
					sKey				kpa_[ ])
	{
		dk = dk_;

		DI::cObj::SP dii = DI::cObj::GetInst( );
		if(dii == 0){	throw ERR_NOTDEVICE; }
		eErr e = CreateDevice(id_);
		if(e != ERR_NOTHING){	throw e; }	//���̓f�o�C�X�̐����ɐ������Ȃ�����
		//�L�[�o�^
		for(DWORD c = 0;  c < numKey_; c++)
		{
			sKeyParam w = {kpa_[c].vKind, kpa_[c].kKind, Off};
			kp.push_back(w);
		}
		
	}
	cInput::~cInput( )
	{
		ML::SafeRelease(&device);
	}
	//	�f�o�C�X�𐶐�����
	cInput::SP cInput::Create(	DeviceKind				dk_,
								DWORD					id_,
								DWORD					numKey_,
								sKey					kpa_[ ])
	{
		return SP( new cInput(dk_, id_, numKey_, kpa_) );
	}
	cInput::SP cInput::CreateKB(	DWORD				numKey_,
									sKey				kpa_[ ])
	{
		return Create(KeyBoard, 0, numKey_, kpa_);
	}
	cInput::SP cInput::CreateGP(	DWORD				id_,
									DWORD				numKey_,
									sKey				kpa_[ ])
	{
		return Create(GamePad, id_, numKey_, kpa_);
	}
	cInput::SP cInput::Link(cInput::SP a, cInput::SP b)
	{
	//�Q�̃f�o�C�X�𓝍����Ďg����悤�ɂ���
		if(a != 0 && b != 0){	a->Link(b);	}	//�L�[�{�[�h�Z�@�Q�[���p�b�h�Z
		else if(a != 0){					}	//�L�[�{�[�h�Z�@�Q�[���p�b�h�~
		else if(b != 0){		a = b;		}	//�L�[�{�[�h�~�@�Q�[���p�b�h�Z
		else{								}	//�L�[�{�[�h�~�@�Q�[���p�b�h�~
		return a;
	}
	//	�X�V
	void cInput::UpDate( )
	{
		switch(dk){		case KeyBoard:		CheckKB( );		break;
						case GamePad:		CheckGP( );		break;	}
		//�֘A�t�����ꂽ�҂�����ꍇ�A������X�V����
		if(link != 0){	link->UpDate( );}
	}
	void cInput::CheckKB( )
	{
		BYTE keyStatus[256];
		ZeroMemory(keyStatus, 256);
		//�f�o�C�X���g�p�\���m�F����
		int acquireCnt = 0;
		BOOL brFlag = FALSE;
		if(FAILED(device->Poll( )))
		{
			do
			{
				if(SUCCEEDED(device->Acquire( ))){		brFlag = TRUE;	}
				if(++acquireCnt > 30){					return;		}
			}while(brFlag == FALSE);
		}
		//	�L�[�{�[�h���擾
		if(FAILED(device->GetDeviceState(sizeof(BYTE) * 256, &keyStatus))){	return;	}

		for(auto it = kp.begin( ); it != kp.end( ); it++)
		{
			if(keyStatus[it->vKind] != 0)
			{
				if(it->kState == Err){it->kState = Off;}
				//�L�[���(�����n)
				switch(it->kState)
				{
					case Off:	//	�����Ă���
					case Up:	//	���������ゾ����
						it->kState = Down;
						break;
					case On:		//	�����Ă���
					case Down:	//	���������ゾ����
						it->kState = On;
						break;
				}
			}
			else
			{
				//�L�[���(���n)
				switch(it->kState)
				{
					case Off:	//	�����Ă���
					case Up:	//	���������ゾ����
						it->kState = Off;
						break;
					case On:		//	�����Ă���
					case Down:	//	���������ゾ����
						it->kState = Up;
						break;
				}
			}
		}
	}
	void cInput::CheckGP( )
	{
		DIJOYSTATE2 js;	//	�L�[�����i�[����
		//�f�o�C�X���g�p�\���m�F����
		int acquireCnt = 0;
		BOOL brFlag = FALSE;
		if(FAILED(device->Poll( )))
		{
			do
			{
				if(SUCCEEDED(device->Acquire( ))){		brFlag = TRUE;		}
				if(++acquireCnt > 30){					return;				}
			}while(brFlag == FALSE);
		}

		//	�p�b�h���擾
		if(FAILED(device->GetDeviceState(sizeof(DIJOYSTATE2), &js)))
		{
			return;
		}
		BOOL keyFlag;
		for(auto it = kp.begin( ); it != kp.end( ); it++)
		{
			//�L�[�R�[�h�ɑΉ������f�[�^�ύX�������s��
			keyFlag = FALSE;
			if(		it->vKind == GPK::X_M){		keyFlag = js.lX < -256;}
			else if(it->vKind == GPK::X_P){		keyFlag = js.lX >  256;}
			else if(it->vKind == GPK::Y_M){		keyFlag = js.lY < -256;}
			else if(it->vKind == GPK::Y_P){		keyFlag = js.lY >  256;}
			else if(it->vKind >= GPK::B00 &&	//�@�{�^���O�O�`�P�S
					it->vKind <= GPK::B15){			keyFlag = js.rgbButtons[it->vKind];}
			else{	continue;	}//���Ή��̃L�[���ݒ肳��Ă���
			if(it->kState == Err){it->kState = Off;}
			
			if(keyFlag != FALSE)
			{
				//�L�[���(�����n)
				switch(it->kState)
				{
					case Off:	//	�����Ă���
					case Up:	//	���������ゾ����
						it->kState = Down;
						break;
					case On:	//	�����Ă���
					case Down:	//	���������ゾ����
						it->kState = On;
						break;
				}
			}
			else
			{
				//�L�[���(���n)
				switch(it->kState)
				{
					case Off:	//	�����Ă���
					case Up:	//	���������ゾ����
						it->kState = Off;
						break;
					case On:	//	�����Ă���
					case Down:	//	���������ゾ����
						it->kState = Up;
						break;
				}
			}
		}
	}

	//	�{�^���̏�Ԃ��m�F
	bool cInput::Check( KK		kk_,
						KS		ks_)
	{
		//�L�[�z��̒�����ΏۂɂȂ�L�[�����o����
		for(auto it = kp.begin( ); it != kp.end( ); it++)
		{
			if(kk_ == it->kKind)		//�o�^�ς݂̃L�[�ꗗ����Y������L�[�����o����
			{
				//�L�[�ƍ�
				if(	(ks_ == Down  &&  it->kState == Down)  ||
					(ks_ == Up    &&  it->kState == Up)    ||
					(ks_ == Off   &&  (it->kState == Up   || it->kState == Off))    ||
					(ks_ == On    &&  (it->kState == Down || it->kState == On))    )
				{
					return true;
				}
				else
				{
					//�֘A�t�����ꂽ�҂�����ꍇ�A������`�F�b�N����
					if(link != 0){	
						if(link->Check(kk_, ks_))
						{
							return true;
						}
					}
				}
			}
		}
		return false;

	}

	static GUID					a_guid;
	static int					a_id;
	BOOL CALLBACK diEnumDevice(		const DIDEVICEINSTANCE* inDevice,
									void* inContext)
	{
		//�f�o�C�X�ԍ�
		DWORD* num = (DWORD*)inContext;
		//�w�肳�ꂽ�ԍ��̃f�o�C�X�ɓ��B
		if((*num) == a_id)
		{
			a_guid = inDevice->guidInstance;	//�f�t�h�c��ۑ�
			return DIENUM_STOP;					//�f�o�C�X�̗񋓂��I������
		}
		else
		{
			(*num)++;						//���Ɉڂ�
			return DIENUM_CONTINUE;			//���̃f�o�C�X�̃`�F�b�N�Ɉڂ�
		}
	}
	BOOL CALLBACK enumObjectsGamePad(	const DIDEVICEOBJECTINSTANCE* inDevice,
										void* inContext)
	{
		IDirectInputDevice8* did = (IDirectInputDevice8*)inContext;
		DIPROPRANGE diprg; 
		//�f�o�C�X�̓��͂͐�Ύ������Ύ��ł���
		if(inDevice->dwType&DIDFT_AXIS)
		{
			diprg.diph.dwSize = sizeof(DIPROPRANGE);
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diprg.diph.dwHow = DIPH_BYID;
			diprg.diph.dwObj = inDevice->dwType;
			diprg.lMin = -1000;
			diprg.lMax = 1000;
			//	�f�o�C�X�̃^�C�v��ݒ肷��
			did->SetProperty(DIPROP_RANGE, &diprg.diph);
		}
		else
		{
			MessageBox(NULL, _T("���̃^�C�v�̃R���g���[���͑Ή����Ă��܂���"), NULL, MB_OK);
		}
		return DIENUM_CONTINUE;
	}
	eErr cInput::CreateDevice(DWORD id_)
	{
		DI::cObj::SP dii = DI::cObj::GetInst( );
		if(dii == 0){	return ERR_NOTDEVICE; }
		if(!(	dk == KeyBoard ||
				dk == GamePad ) ){
			return ERR_UNNONTYPE;
		}

		struct {
			DWORD				dType;
			LPCDIDATAFORMAT		dFormat;
			DWORD				cLevel;

		}kind[] = {
			//cInput::KeyBoard
			{DI8DEVCLASS_KEYBOARD, &c_dfDIKeyboard,  DISCL_NONEXCLUSIVE | DISCL_FOREGROUND},
			//cInput::GamePad
			{DI8DEVCLASS_GAMECTRL, &c_dfDIJoystick2, DISCL_EXCLUSIVE	| DISCL_FOREGROUND},
		};

//		return ERR_UNNONTYPE;//zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz

	//�f�o�C�X�̌��o���s��----------------------------------------------------------
		//�w��f�o�C�X���o�i�O���[�o���ϐ��@a_guid�@����с@a_id�@�ɒ��Ӂj
		DWORD deviceCount = 0;
		a_id = id_;
		if(FAILED(dii->Object( )->EnumDevices(kind[dk].dType, 
			diEnumDevice, &deviceCount, DIEDFL_ATTACHEDONLY))){						return ERR_ENUMDEVICE;	}
		if(deviceCount < id_){														return ERR_ENUMDEVICE;	}

	//�f�o�C�X�̐������s��----------------------------------------------------------
		//DirectInput�f�o�C�X�̐���
		//INPUTDEVICEFLAG* inputDeviceWork;
		//switch(inDeviceType){
		//	case eDeviceType::eKeyBoard:	inputDeviceWork = deviceFlags_Keybd;	break;		//�@�L�[�{�[�h
		//	case eDeviceType::eGamePad:		inputDeviceWork = deviceFlags_Pad;		break;		//�@�Q�[���R���g���[���[
		//}

	//���̓f�o�C�X�𐶐�����
		if(FAILED(dii->Object( )->CreateDevice( a_guid, &device, NULL))){		return ERR_CREATEDEVICE;	}
	//���̓f�o�C�X�̃t�H�[�}�b�g��ݒ肷��
		if(FAILED(device->SetDataFormat(kind[dk].dFormat))){					return ERR_DATAFORMAT;		}
	//���̓f�o�C�X�̋������x����ݒ肷��
		if(FAILED(device->SetCooperativeLevel(dii->Wnd( ), kind[dk].cLevel))){	return ERR_COOPLEVEL;		}


	//���̓Q�[���p�b�h�̏ꍇ�A�����L�[�ɐ�Ύ��������͑��Ύ����g�p�\�Ȃ��Ǝ����m�F����B
		switch(dk)
		{
			case GamePad:
				//��Ύ�����ё��Ύ����g�p�\�ȏꍇ�A���̐ݒ���s��
				if(FAILED(device->EnumObjects(	enumObjectsGamePad,
												device,
												DIDFT_AXIS)))
				{
					return ERR_GPAXIS;
				}
				break;
			case KeyBoard:
				{
					DIPROPDWORD dipdw;
					dipdw.diph.dwSize		= sizeof(DIPROPDWORD);
					dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
					dipdw.diph.dwObj		= 0;
					dipdw.diph.dwHow		= DIPH_DEVICE;
					dipdw.dwData			= 8;//�����ɔ���ł���L�[�̐��ɂȂ�̂��ȁH
					if(FAILED(device->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
					{
						return ERR_KBPROPERTY;
					}
				}			
				break;
		}
		return ERR_NOTHING;
	}

}