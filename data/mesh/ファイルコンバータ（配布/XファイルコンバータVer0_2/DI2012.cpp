#include "DI2012.h"


namespace DI
{
	cObj::WP cObj::winst;	//	１つしか存在できないインスタンス

	cObj::cObj(HWND wnd_)
	{
		wnd = wnd_;
		if(FAILED(DirectInput8Create(	GetModuleHandle(NULL),
										DIRECTINPUT_VERSION,
										IID_IDirectInput8,
										(VOID**)&di,
										NULL)))
		{
			//	デバイス生成失敗
			throw ERR_CREATE;
		}
	}
	cObj::~cObj( )
	{
	//	DIオブジェクトの解放
		ML::SafeRelease(&di);
	}
	//	DIライブラリを生成する
	cObj::SP cObj::Create(	HWND		hw_)	//	ウィンドウハンドル
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
		if(e != ERR_NOTHING){	throw e; }	//入力デバイスの生成に成功しなかった
		//キー登録
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
	//	デバイスを生成する
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
	//２つのデバイスを統合して使えるようにする
		if(a != 0 && b != 0){	a->Link(b);	}	//キーボード〇　ゲームパッド〇
		else if(a != 0){					}	//キーボード〇　ゲームパッド×
		else if(b != 0){		a = b;		}	//キーボード×　ゲームパッド〇
		else{								}	//キーボード×　ゲームパッド×
		return a;
	}
	//	更新
	void cInput::UpDate( )
	{
		switch(dk){		case KeyBoard:		CheckKB( );		break;
						case GamePad:		CheckGP( );		break;	}
		//関連付けされた者がある場合、それも更新する
		if(link != 0){	link->UpDate( );}
	}
	void cInput::CheckKB( )
	{
		BYTE keyStatus[256];
		ZeroMemory(keyStatus, 256);
		//デバイスが使用可能か確認する
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
		//	キーボード情報取得
		if(FAILED(device->GetDeviceState(sizeof(BYTE) * 256, &keyStatus))){	return;	}

		for(auto it = kp.begin( ); it != kp.end( ); it++)
		{
			if(keyStatus[it->vKind] != 0)
			{
				if(it->kState == Err){it->kState = Off;}
				//キー状態(押下系)
				switch(it->kState)
				{
					case Off:	//	離していた
					case Up:	//	離した直後だった
						it->kState = Down;
						break;
					case On:		//	押していた
					case Down:	//	押した直後だった
						it->kState = On;
						break;
				}
			}
			else
			{
				//キー状態(離系)
				switch(it->kState)
				{
					case Off:	//	離していた
					case Up:	//	離した直後だった
						it->kState = Off;
						break;
					case On:		//	押していた
					case Down:	//	押した直後だった
						it->kState = Up;
						break;
				}
			}
		}
	}
	void cInput::CheckGP( )
	{
		DIJOYSTATE2 js;	//	キー情報を格納する
		//デバイスが使用可能か確認する
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

		//	パッド情報取得
		if(FAILED(device->GetDeviceState(sizeof(DIJOYSTATE2), &js)))
		{
			return;
		}
		BOOL keyFlag;
		for(auto it = kp.begin( ); it != kp.end( ); it++)
		{
			//キーコードに対応したデータ変更処理を行う
			keyFlag = FALSE;
			if(		it->vKind == GPK::X_M){		keyFlag = js.lX < -256;}
			else if(it->vKind == GPK::X_P){		keyFlag = js.lX >  256;}
			else if(it->vKind == GPK::Y_M){		keyFlag = js.lY < -256;}
			else if(it->vKind == GPK::Y_P){		keyFlag = js.lY >  256;}
			else if(it->vKind >= GPK::B00 &&	//　ボタン００～１４
					it->vKind <= GPK::B15){			keyFlag = js.rgbButtons[it->vKind];}
			else{	continue;	}//未対応のキーが設定されている
			if(it->kState == Err){it->kState = Off;}
			
			if(keyFlag != FALSE)
			{
				//キー状態(押下系)
				switch(it->kState)
				{
					case Off:	//	離していた
					case Up:	//	離した直後だった
						it->kState = Down;
						break;
					case On:	//	押していた
					case Down:	//	押した直後だった
						it->kState = On;
						break;
				}
			}
			else
			{
				//キー状態(離系)
				switch(it->kState)
				{
					case Off:	//	離していた
					case Up:	//	離した直後だった
						it->kState = Off;
						break;
					case On:	//	押していた
					case Down:	//	押した直後だった
						it->kState = Up;
						break;
				}
			}
		}
	}

	//	ボタンの状態を確認
	bool cInput::Check( KK		kk_,
						KS		ks_)
	{
		//キー配列の中から対象になるキーを検出する
		for(auto it = kp.begin( ); it != kp.end( ); it++)
		{
			if(kk_ == it->kKind)		//登録済みのキー一覧から該当するキーを検出した
			{
				//キー照合
				if(	(ks_ == Down  &&  it->kState == Down)  ||
					(ks_ == Up    &&  it->kState == Up)    ||
					(ks_ == Off   &&  (it->kState == Up   || it->kState == Off))    ||
					(ks_ == On    &&  (it->kState == Down || it->kState == On))    )
				{
					return true;
				}
				else
				{
					//関連付けされた者がある場合、それもチェックする
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
		//デバイス番号
		DWORD* num = (DWORD*)inContext;
		//指定された番号のデバイスに到達
		if((*num) == a_id)
		{
			a_guid = inDevice->guidInstance;	//ＧＵＩＤを保存
			return DIENUM_STOP;					//デバイスの列挙を終了する
		}
		else
		{
			(*num)++;						//次に移る
			return DIENUM_CONTINUE;			//次のデバイスのチェックに移る
		}
	}
	BOOL CALLBACK enumObjectsGamePad(	const DIDEVICEOBJECTINSTANCE* inDevice,
										void* inContext)
	{
		IDirectInputDevice8* did = (IDirectInputDevice8*)inContext;
		DIPROPRANGE diprg; 
		//デバイスの入力は絶対軸か相対軸である
		if(inDevice->dwType&DIDFT_AXIS)
		{
			diprg.diph.dwSize = sizeof(DIPROPRANGE);
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diprg.diph.dwHow = DIPH_BYID;
			diprg.diph.dwObj = inDevice->dwType;
			diprg.lMin = -1000;
			diprg.lMax = 1000;
			//	デバイスのタイプを設定する
			did->SetProperty(DIPROP_RANGE, &diprg.diph);
		}
		else
		{
			MessageBox(NULL, _T("このタイプのコントローラは対応していません"), NULL, MB_OK);
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

	//デバイスの検出を行う----------------------------------------------------------
		//指定デバイス検出（グローバル変数　a_guid　および　a_id　に注意）
		DWORD deviceCount = 0;
		a_id = id_;
		if(FAILED(dii->Object( )->EnumDevices(kind[dk].dType, 
			diEnumDevice, &deviceCount, DIEDFL_ATTACHEDONLY))){						return ERR_ENUMDEVICE;	}
		if(deviceCount < id_){														return ERR_ENUMDEVICE;	}

	//デバイスの生成を行う----------------------------------------------------------
		//DirectInputデバイスの生成
		//INPUTDEVICEFLAG* inputDeviceWork;
		//switch(inDeviceType){
		//	case eDeviceType::eKeyBoard:	inputDeviceWork = deviceFlags_Keybd;	break;		//　キーボード
		//	case eDeviceType::eGamePad:		inputDeviceWork = deviceFlags_Pad;		break;		//　ゲームコントローラー
		//}

	//入力デバイスを生成する
		if(FAILED(dii->Object( )->CreateDevice( a_guid, &device, NULL))){		return ERR_CREATEDEVICE;	}
	//入力デバイスのフォーマットを設定する
		if(FAILED(device->SetDataFormat(kind[dk].dFormat))){					return ERR_DATAFORMAT;		}
	//入力デバイスの協調レベルを設定する
		if(FAILED(device->SetCooperativeLevel(dii->Wnd( ), kind[dk].cLevel))){	return ERR_COOPLEVEL;		}


	//入力ゲームパッドの場合、方向キーに絶対軸もしくは相対軸が使用可能なこと事を確認する。
		switch(dk)
		{
			case GamePad:
				//絶対軸および相対軸が使用可能な場合、軸の設定を行う
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
					dipdw.dwData			= 8;//同時に判定できるキーの数になるのかな？
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