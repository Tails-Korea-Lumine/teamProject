#include "DI2014_Ver3_4.h"

namespace DI_
{
	namespace  KS {
		enum Type {	//キー状態	
			Err,
			On,		//押している間中
			Off,	//離している間中
			Down,	//押した直後（１フレームのみ反応）
			Up,		//離した直後（１フレームのみ反応）
		};
	}
	struct  StickInfo {
		float                angle;		//アナログスティックの角度（RT0度　DW90度のラジアン）
		float                volume;	//アナログスティックの強弱0.0f～1.0f
		ML::Vec2             axis;		//軸毎の量-1.0f～+1.0f
		KS::Type       xm;		//ボタンに見立てた状態　左
		KS::Type       xp;		//ボタンに見立てた状態　右
		KS::Type       ym;		//ボタンに見立てた状態　上
		KS::Type       yp;		//ボタンに見立てた状態　下
	};
	namespace  But {
		enum Type {	//プログラム上で使用できるキーの種類
					//○,
			B1,
			//×
			B2,
			//△
			B3,
			//□
			B4,
			L1, R1, L2, R2,
			ST, SE, L3, R3,
			//←
			L,		//ハットスイッチ
					//→
			R,
			//↑
			U,
			//↓
			D,
			S0, S1, S2, S3, S4, S5, S6, S7, S8, S9,
			KMax		//　ボタン総数
		};
	}
	namespace  Axis {
		enum Type {	//アナログスティックにボタンと同様にアクセスるする場合に使用
					//←
			L,		//ハットスイッチ
					//→
			R,
			//↑
			U,
			//↓
			D,
			KMax		//　ボタン総数
		};
	}

	namespace GPBut	//ゲームパッドのキーを表す
	{
		enum Type {
			B01, B02, B03, B04, B05, B06, B07, B08,
			B09, B10, B11, B12, B13, B14, B15, B16,
			HSL, HSR, HSU, HSD,		//ハットスイッチ 
			KMax				//　ボタン総数
		};
	}
	namespace MB	//マウスのボタンを表す
	{
		enum Type {  //マウスボタンの種類 8個まで認識可能
			LB, RB, CB, RESERVED1,
			RESERVED2, RESERVED3, RESERVED4, RESERVED5,
			kMax
		};
	}
	//キー設定キーボード用
	struct  KeyDataKB {
		DWORD           vKey;	//キーボードのバーチャルキー
		But::Type       pKey;	//ボタンの種類（ＰＧ上での名称）
	};
	struct  AnalogAxisKB {
		DWORD           vKeyL;	//キーボードのバーチャルキー
		DWORD           vKeyR;	//キーボードのバーチャルキー
		DWORD           vKeyU;	//キーボードのバーチャルキー
		DWORD           vKeyD;	//キーボードのバーチャルキー
	};
	//キー設定ゲームパッド用
	struct  KeyDataGP {
		GPBut::Type      gKey;	//ゲームパッドのボタン
		But::Type        pKey;	//ボタンの種類（ＰＧ上での名称）
	};

	//例外として投げられるエラー情報
	namespace Err{
		enum Type
		{
			NOTHING,		//エラーなし
			CREATE,			//生成失敗
			NOTDEVICE,		//デバイスが取得的ない
			UNNONTYPE,		//不明なデバイス種類
			ENUMDEVICE,		//デバイスの列挙に失敗
			CREATEDEVICE,	//デバイスの生成失敗
			DATAFORMAT,		//データフォーマットの設定失敗
			COOPLEVEL,		//協調レベル設定失敗
			GPAXIS,			//
			KBPROPERTY,
		};
	}

	class  Obj
	{
		IDirectInput8*  di;
		HWND  wnd;

		typedef  weak_ptr<Obj>  WP;
		static  Obj::WP  winst;		//	１つしか存在できないインスタンスのウィークポインタ
		Obj();
		bool  Initialize(HWND wnd_);
	public:
		typedef  shared_ptr<Obj>  SP;
		~Obj();
		//	DI_ライブラリを生成する
		static  Obj::SP  Create(HWND  wnd_);
		static  Obj::SP  GetInst();
		HWND  Wnd(){ return  wnd; }
		IDirectInput8&  DInput(){ return  *di; }
	};
	class  Input
	{
	public:
	private:
		enum  DeviceKind{ KeyBoard, GamePad, UnNon };	//デバイスの種類
		struct sKeyParam{
			DWORD             dKey;		//実際のボタンまたはキーボードのバーチャルキー
			But::Type     pKey;		//ボタンの種類（ＰＧ上での名称）
			KS::Type      state;	//キーの状態
		};
		IDirectInputDevice8*  device;
		DeviceKind            dk;
		vector<sKeyParam>     buttons;
		vector<DWORD>         kbAxisL;	//キーボード専用方向キー
		vector<DWORD>         kbAxisR;	//キーボード専用方向キー
		vector<DWORD>         kbPov4;	//キーボード専用POVキー
		StickInfo         ls, rs;

		Input();
		bool  Initialize(
			DeviceKind    dk_,
			DWORD         id_,
			AnalogAxisKB* axisL_,
			AnalogAxisKB* axisR_,
			DWORD         numButton_,
			KeyDataKB     buttonKB_[],
			KeyDataGP     buttonGP_[]);
		Err::Type  CreateDevice(DWORD  id_);

		void  CheckKB();
		void  CheckGP();
		void  CheckUnNon();
		bool  CheckBT(But::Type  kk_, KS::Type  ks_);	//検出するボタンの状態
		bool  CheckStickL(Axis::Type  kk_, KS::Type  ks_);
		bool  CheckStickR(Axis::Type  kk_, KS::Type  ks_);


	public:
		~Input();
		typedef  shared_ptr<Input>  SP;
		typedef  weak_ptr<Input>  WP;
		//	デバイスを生成する
		static  SP  CreateKB(
			AnalogAxisKB	axisL_,
			AnalogAxisKB	axisR_,
			DWORD			numButton_,
			KeyDataKB	button_[]);
		static  SP  CreateGP(
			DWORD			id_,
			DWORD			numButton_,
			KeyDataGP	button_[]);
		//	更新
		void  UpDate();
		//	ボタンの状態を確認
		bool  Button_On(But::Type  kk_);
		bool  Button_Off(But::Type  kk_);
		bool  Button_Down(But::Type  kk_);
		bool  Button_Up(But::Type  kk_);
		KS::Type  CheckBT(But::Type  kk_);
		//	アナログスティックの入力を得る
		StickInfo  StickL();
		StickInfo  StickR();

		//	アナログスティックの４方向をボタンに見立てて使用する
		//	ボタンのＯＮ　ＯＦＦでしかないのでアナログの情報にはならない
		bool  StickL_On(Axis::Type  kk_);
		bool  StickL_Off(Axis::Type  kk_);
		bool  StickL_Down(Axis::Type  kk_);
		bool  StickL_Up(Axis::Type  kk_);
		bool  StickR_On(Axis::Type  kk_);
		bool  StickR_Off(Axis::Type  kk_);
		bool  StickR_Down(Axis::Type  kk_);
		bool  StickR_Up(Axis::Type  kk_);


	private:
		static  SP  Create(
			DeviceKind		dk_,
			DWORD			id_,
			AnalogAxisKB*	axisL_,
			AnalogAxisKB*	axisR_,
			DWORD			numButton_,
			KeyDataKB		buttonKB_[],
			KeyDataGP		buttonGP_[]);
		SP  link;

	public:
		//２つのデバイスを統合する
		//安全性を考えるなら、クラスメソッドのLinkを使う事
		SP  Link(SP  l){
			if (this == l.get()){ return 0; }
			else{ SP w = link; link = l; return link; }
		}
		//２つのデバイスを統合する
		//片方のデバイスが生成できていない場合の対応あり
		static  Input::SP  Link(SP  a, SP  b);
	};
	//--------------------------------
	//add koyanagi
	class Mouse{
		//    IDirectInputDevice8*	device;
		//--------------------------------
		//マウス
		LPDIRECTINPUTDEVICE8 mouse;
		DIMOUSESTATE2 mouseState;
		POINT         mouseCPos;
		POINT         mouseWPos;
		long          mouseWheel;
		KS::Type      mouseButton[MB::kMax];
		Err::Type     init(float, float);

		float         scaleX, scaleY;
		//--------------------------------
	public:
		POINT& Distance();
		POINT& CursorPos();
		int    Wheel();
		KS::Type     CheckBT(MB::Type mb_);
		//	ボタンの状態を確認
		bool  Button_On(MB::Type  mb_);
		bool  Button_Off(MB::Type  mb_);
		bool  Button_Down(MB::Type  mb_);
		bool  Button_Up(MB::Type  mb_);
		bool  CheckBT(MB::Type		mb_, KS::Type		ks_);	//検出するボタンの状態
		//--------------------------------
		Mouse();
		~Mouse();
		typedef shared_ptr<Mouse> SP;
		typedef weak_ptr<Mouse> WP;
		//生成
		static SP   Create(float scaleX_ = 1.f, float scaleY_ = 1.f);
		//	更新
		void UpDate();

	};
}
namespace DI_
{
	const  int  analogMax  =  1000;		//アナログ入力のボリュームは-1000～1000とする
	const  float  digToAnalog  =  1.0f / analogMax;	//

	Obj::WP Obj::winst;	//	１つしか存在できないインスタンス

	Obj::Obj( )
	{
		wnd = 0;
		di = nullptr;
	}
	bool Obj::Initialize(HWND wnd_)
	{
		wnd = wnd_;
		if(FAILED(DirectInput8Create(	GetModuleHandle(nullptr),
										DIRECTINPUT_VERSION,
										IID_IDirectInput8,
										(VOID**)&di,
										nullptr)))
		{
			//	デバイス生成失敗
			return false;
		}
		return true;
	}

	Obj::~Obj( )
	{
	//	DI_オブジェクトの解放
		ML::SafeRelease(&di);
	}
	//	DI_ライブラリを生成する
	Obj::SP Obj::Create(	HWND		hw_)	//	ウィンドウハンドル
	{
		if(winst.expired( )){
			Obj::SP sp = Obj::SP(new Obj( ));
			if( sp->Initialize( hw_ ) ){
				winst = sp;
				return sp;
			}
		}
		return 0;
	}
	Obj::SP Obj::GetInst( )
	{
		if(!winst.expired( )){
			return winst.lock( );
		}
		return 0;
	}
	Input::Input(	)
	{
		device = nullptr;
	}
	bool Input::Initialize(	
			DeviceKind			dk_,
			DWORD				id_,
			AnalogAxisKB*	axisL_,
			AnalogAxisKB*	axisR_,
			DWORD				numButton_,
			KeyDataKB		buttonKB_[],
			KeyDataGP		buttonGP_[])
	{
		dk = dk_;
		StickInfo  w = { 0.0f, 0.0f, ML::Vec2(0, 0), KS::Off, KS::Off, KS::Off, KS::Off };
		ls = w;
		rs = w;

		Obj::SP dii = Obj::GetInst( );
		if(dii == 0){	return false; }
		Err::Type  e = this->CreateDevice(id_);
		//入力デバイスの生成に成功しなかった
		if(e != Err::NOTHING){
			dk = UnNon;
			return true;	//仕様変更によりtrueとする
		}	
		//キー登録
		if(axisL_){
			kbAxisL.push_back(axisL_->vKeyL);
			kbAxisL.push_back(axisL_->vKeyR);
			kbAxisL.push_back(axisL_->vKeyU);
			kbAxisL.push_back(axisL_->vKeyD);
		}
		if(axisR_){
			kbAxisR.push_back(axisR_->vKeyL);
			kbAxisR.push_back(axisR_->vKeyR);
			kbAxisR.push_back(axisR_->vKeyU);
			kbAxisR.push_back(axisR_->vKeyD);
		}

		if(buttonKB_){
			for(DWORD c = 0;  c < numButton_; c++)
			{
				sKeyParam w = { buttonKB_[c].vKey, buttonKB_[c].pKey, KS::Off };
				buttons.push_back(w);
			}
		}
		if(buttonGP_){
			for(DWORD c = 0;  c < numButton_; c++)
			{
				sKeyParam w = { (DWORD)buttonGP_[c].gKey, buttonGP_[c].pKey, KS::Off };
				buttons.push_back(w);
			}
		}

		return true;
	}
	Input::~Input( )
	{
		ML::SafeRelease(&device);
	}
	//	デバイスを生成する
	Input::SP Input::Create(	
			DeviceKind			dk_,
			DWORD				id_,
			AnalogAxisKB*		axisL_,
			AnalogAxisKB*		axisR_,
			DWORD				numButton_,
			KeyDataKB			buttonKB_[],
			KeyDataGP			buttonGP_[])
	{
		auto sp = SP(new Input( ));
		if(sp){
			if( sp->Initialize(dk_,  id_,  axisL_,  axisR_,  numButton_,  buttonKB_,  buttonGP_) )
			{
				return sp;
			}
		}
		return 0;
	}

	Input::SP Input::CreateKB(
				AnalogAxisKB		axisL_,
				AnalogAxisKB		axisR_,
				DWORD				numButton_,
				KeyDataKB			button_[])
	{
		AnalogAxisKB*  aL = nullptr;
		AnalogAxisKB*  aR = nullptr;
		if(axisL_.vKeyL && axisL_.vKeyR && axisL_.vKeyU && axisL_.vKeyD){	aL = &axisL_;} 
		if(axisR_.vKeyL && axisR_.vKeyR && axisR_.vKeyU && axisR_.vKeyD){	aR = &axisR_;} 
		return  Create(KeyBoard,  0,  aL,  aR,  numButton_,  button_,  nullptr);
	}
	Input::SP Input::CreateGP(
				DWORD				id_,
				DWORD				numButton_,
				KeyDataGP		button_[])
	{
		return Create(GamePad,  id_,  nullptr,  nullptr,  numButton_,  nullptr,  button_);
	}
	Input::SP Input::Link(Input::SP a, Input::SP b)
	{
	//２つのデバイスを統合して使えるようにする
		if(a != 0 && b != 0){	a->Link(b);	}	//キーボード〇　ゲームパッド〇
		else if(a != 0){					}	//キーボード〇　ゲームパッドB2
		else if(b != 0){		a = b;		}	//キーボードB2　ゲームパッド〇
		else{								}	//キーボードB2　ゲームパッドB2
		return a;
	}
	//	更新
	void Input::UpDate( )
	{
		switch(dk){		case KeyBoard:		CheckKB( );		break;
						case GamePad:		CheckGP();		break;
						case UnNon:		CheckUnNon();	break;	}
		//関連付けされた者がある場合、それも更新する
		if(link != 0){	link->UpDate( );}
	}
	//ボタンの状態を更新する
	KS::Type  UpdateButtonState(bool  nbs_, KS::Type  old_)
	{
		if(nbs_){
			if (old_ == KS::Err){ old_ = KS::Off; }
			//キー状態(押下系)
			switch(old_){
				case KS::Off:
				case KS::Up:
					old_ = KS::Down;	break;	//	離していた&離した直後だった
				case KS::On:
				case KS::Down:
					old_ = KS::On;	break;	//	押していた&押した直後だった
			}
		}else{
			//キー状態(離系)
			switch(old_){
				case KS::Off:
				case KS::Up:
					old_ = KS::Off;	break;	//	離していた&離した直後だった
				case KS::On:
				case KS::Down:
					old_ = KS::Up;	break;	//	押していた&押した直後だった
			}
		}
		return old_;
	}
	void  SIUpdate(StickInfo&  si_, float  ax_, float  ay_)
	{
		si_.axis.x  =  ax_;
		si_.axis.y  =  ay_;

		//入力量が少ない場合、ニュートラルとする
		if( -0.1f < si_.axis.x  &&  si_.axis.x < 0.1f &&
			-0.1f < si_.axis.y  &&  si_.axis.y < 0.1f ){
				si_.axis.x = 0;
				si_.axis.y = 0;
		}

		//角度と長さに変換したアナログスティック入力
		if(si_.axis.x  !=  0 || si_.axis.y  !=  0){//スティックから傾きを検出できる
			//右を0度、下を90度とした角度を求める
			si_.angle  =  atan2(si_.axis.y, si_.axis.x);//傾きから角度を算出
			//レバーの傾き量は±0～1に置き換える
			si_.volume  =  sqrt(si_.axis.x * si_.axis.x  +  si_.axis.y * si_.axis.y);
			si_.volume  =  min(si_.volume, 1.0f);
		}
		else{
			si_.volume  =  0.0f;
		}
		//４方向ボタンのイメージでマッピング
		si_.xm  =  UpdateButtonState(ax_ < -0.1f,  si_.xm);
		si_.xp  =  UpdateButtonState(ax_ > +0.1f,  si_.xp);
		si_.ym  =  UpdateButtonState(ay_ < -0.1f,  si_.ym);
		si_.yp  =  UpdateButtonState(ay_ > +0.1f,  si_.yp);
	}
	void Input::CheckKB( )
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
		//	方向入力に登録されたキーへの対応
		float  ax_L=0,  ay_L=0;
		if(kbAxisL.size( )==4){
			if(keyStatus[kbAxisL[0]] != 0){	ax_L -= 1;}
			if(keyStatus[kbAxisL[1]] != 0){	ax_L += 1;}
			if(keyStatus[kbAxisL[2]] != 0){	ay_L -= 1;}
			if(keyStatus[kbAxisL[3]] != 0){	ay_L += 1;}
		}
		float  ax_R=0,  ay_R=0;
		if(kbAxisR.size( )==4){
			if(keyStatus[kbAxisR[0]] != 0){	ax_R -= 1;}
			if(keyStatus[kbAxisR[1]] != 0){	ax_R += 1;}
			if(keyStatus[kbAxisR[2]] != 0){	ay_R -= 1;}
			if(keyStatus[kbAxisR[3]] != 0){	ay_R += 1;}
		}

		//LRスティック情報更新
		SIUpdate(ls,  ax_L,  ay_L);
		SIUpdate(rs,  ax_R,  ay_R);
		//　ボタンとして登録されたキーへの対応
		for(auto it = begin(buttons); it != end(buttons); ++it){
			it->state  =  UpdateButtonState(keyStatus[it->dKey] != 0,  it->state);
		}
	}
	
	bool  KeyCheck(KS::Type  state_, KS::Type  m_)
	{
		namespace  KS = KS;
		//キー照合
		if(	(m_ == KS::Down  &&  state_ == KS::Down)  ||
			(m_ == KS::Up    &&  state_ == KS::Up)    ||
			(m_ == KS::Off   &&  (state_ == KS::Up   || state_ == KS::Off))  ||
			(m_ == KS::On    &&  (state_ == KS::Down || state_ == KS::On))   ){
			return true;
		}
		return false;
	}


	bool  Input::StickL_On(Axis::Type  kk_){   return  this->CheckStickL(kk_, KS::On); }
	bool  Input::StickL_Off(Axis::Type  kk_){  return  this->CheckStickL(kk_, KS::Off); }
	bool  Input::StickL_Down(Axis::Type  kk_){ return  this->CheckStickL(kk_, KS::Down); }
	bool  Input::StickL_Up(Axis::Type  kk_){   return  this->CheckStickL(kk_, KS::Up); }
	bool  Input::StickR_On(Axis::Type  kk_){   return  this->CheckStickR(kk_, KS::On); }
	bool  Input::StickR_Off(Axis::Type  kk_){  return  this->CheckStickR(kk_, KS::Off); }
	bool  Input::StickR_Down(Axis::Type  kk_){ return  this->CheckStickR(kk_, KS::Down); }
	bool  Input::StickR_Up(Axis::Type  kk_){   return  this->CheckStickR(kk_, KS::Up); }

	bool Input::CheckStickL(Axis::Type  kk_, KS::Type  ks_)
	{
		KS::Type   w;
		if(     kk_  ==  Axis::L){	w  =  StickL( ).xm;}
		else if(kk_  ==  Axis::R){	w  =  StickL( ).xp;}
		else if(kk_  ==  Axis::U){	w  =  StickL( ).ym;}
		else if(kk_  ==  Axis::D){	w  =  StickL( ).yp;}
		else{						return  false;}
		return  KeyCheck(w,  ks_);
	}
	bool Input::CheckStickR(Axis::Type  kk_, KS::Type  ks_)
	{
		KS::Type   w;
		if(     kk_  ==  Axis::L){	w  =  StickR( ).xm;}
		else if(kk_  ==  Axis::R){	w  =  StickR( ).xp;}
		else if(kk_  ==  Axis::U){	w  =  StickR( ).ym;}
		else if(kk_  ==  Axis::D){	w  =  StickR( ).yp;}
		else{						return  false;}
		return  KeyCheck(w,  ks_);
	}

	StickInfo  Input::StickL()
	{
		StickInfo  me = this->ls;
		StickInfo  linkRtv;
		if(link != nullptr){	
			linkRtv = link->StickL( );
			if(me.volume  <  linkRtv.volume){
				return linkRtv;
			}
		}
		return me;
	}
	StickInfo  Input::StickR()
	{
		StickInfo  me = this->rs;
		StickInfo  linkRtv;
		if(link != nullptr){	
			linkRtv = link->StickR( );
			if(me.volume  <  linkRtv.volume){	return linkRtv;}
		}
		return me;
	}
	void  Input::CheckUnNon()
	{
		//アナログスティック対応
		SIUpdate(ls, 0, 0);
		SIUpdate(rs, 0, 0);

		//ボタンへの対応
		for (auto it = begin(buttons); it != end(buttons); ++it)
		{
			it->state = UpdateButtonState(false, it->state);
		}
	}


	void Input::CheckGP( )
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
		//アナログスティック対応
		SIUpdate(ls,  js.lX * digToAnalog,  js.lY  * digToAnalog);
		SIUpdate(rs,  js.lZ * digToAnalog,  js.lRz * digToAnalog);


		//POV対応
		int m = 8;
		if(js.rgdwPOV[0] != 0xFFFFFFFF){	m = js.rgdwPOV[0] / 4500;}//0-7
		bool xym[9][4] = {
		//   L R U D
			{0,0,1,0},		//0  UP
			{0,1,1,0},		//1  UPRT
			{0,1,0,0},		//2  RT
			{0,1,0,1},		//3  DWRT
			{0,0,0,1},		//4  DW
			{1,0,0,1},		//5  DWLE
			{1,0,0,0},		//6  LE
			{1,1,0,0},		//7  UPLE
			{0,0,0,0},		//8  NT
		};
		//ボタンへの対応
		bool keyFlag;
		for(auto it = begin(buttons); it != end(buttons); ++it)
		{
			//キーコードに対応したデータ変更処理を行う
			keyFlag = false;
			//POV対応
			if(		it->dKey == GPBut::HSL){		keyFlag = xym[m][0];}
			else if(it->dKey == GPBut::HSR){		keyFlag = xym[m][1];}
			else if(it->dKey == GPBut::HSU){		keyFlag = xym[m][2];}
			else if(it->dKey == GPBut::HSD){		keyFlag = xym[m][3];}
			//ボタン００～１４
			else if(it->dKey >= GPBut::B01 &&	
					it->dKey <= GPBut::B16){		keyFlag = js.rgbButtons[it->dKey] != 0;}
			//未対応のキーが設定されている
			else{	continue;	}
			it->state  =  UpdateButtonState(keyFlag,  it->state);
		}
	}

	//２つのキーを統合した時、打倒な状態を返す
	KS::Type  IntegrationKey(KS::Type  a_, KS::Type  b_)
	{
		namespace KS = KS;
		KS::Type ret = KS::Off;

		KS::Type  mat[5][5] = {
			//Err  On  Off   Down  Up
			{KS::Err,  KS::On, KS::Off,  KS::Down, KS::Up}, //Err
			{KS::On,   KS::On, KS::On,   KS::On,   KS::On}, //On
			{KS::Off,  KS::On, KS::Off,  KS::Down, KS::Up}, //Off
			{KS::Down, KS::On, KS::Down, KS::Down, KS::On}, //Down
			{KS::Up,   KS::On, KS::Up,   KS::On,   KS::Up}, //Up
		};
		return  mat[a_][b_];
	}
	KS::Type	 Input::CheckBT(But::Type		kk_)
	{
		KS::Type ret = KS::Off;

		//キー配列の中から対象になるキーを検出する
		for(auto it = begin(buttons); it != end(buttons); ++it)
		{
			if(kk_ == it->pKey)		//登録済みのキー一覧から該当するキーを検出した
			{
				ret = (KS::Type)it->state;
			}
			if(link != 0){
				ret = IntegrationKey(ret, link->CheckBT(kk_));
			}
		}
		return ret;
	}



	bool  Input::Button_On(But::Type  kk_){   return  this->CheckBT(kk_, KS::On); }
	bool  Input::Button_Off(But::Type  kk_){  return  this->CheckBT(kk_, KS::Off); }
	bool  Input::Button_Down(But::Type  kk_){ return  this->CheckBT(kk_, KS::Down); }
	bool  Input::Button_Up(But::Type  kk_){   return  this->CheckBT(kk_, KS::Up); }
	//	ボタンの状態を確認
	bool Input::CheckBT(But::Type  kk_, KS::Type  ks_)
	{
		KS::Type  ks = CheckBT(kk_);
		//キー照合
		if ((ks_ == KS::Down  &&  ks == KS::Down) ||
			(ks_ == KS::Up    &&  ks == KS::Up)   ||
			(ks_ == KS::Off   && (ks == KS::Up    || ks == KS::Off)) ||
			(ks_ == KS::On    && (ks == KS::Down  || ks == KS::On))){
			return true;
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
			(*num)++;
			return DIENUM_STOP;					//デバイスの列挙を終了する
		}
		else
		{
			(*num)++;
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
			diprg.lMin = -analogMax;
			diprg.lMax =  analogMax;
			//	デバイスのタイプを設定する
			did->SetProperty(DIPROP_RANGE, &diprg.diph);
		}
		else
		{
			MessageBox(nullptr, "このタイプのコントローラは対応していません", nullptr, MB_OK);
		}
		return DIENUM_CONTINUE;
	}
	Err::Type Input::CreateDevice(DWORD id_)
	{
		Obj::SP dii = Obj::GetInst( );
		if(dii == 0){	return Err::NOTDEVICE; }
		if(!(	dk == KeyBoard ||
				dk == GamePad ) ){
			return Err::UNNONTYPE;
		}

		struct {
			DWORD				dType;
			LPCDIDATAFORMAT		dFormat;
			DWORD				cLevel;

		}kind[] = {
			//Input::KeyBoard
			{DI8DEVCLASS_KEYBOARD, &c_dfDIKeyboard,  DISCL_NONEXCLUSIVE | DISCL_FOREGROUND},
			//Input::GamePad
			{DI8DEVCLASS_GAMECTRL, &c_dfDIJoystick2, DISCL_EXCLUSIVE	| DISCL_FOREGROUND},
		};

	//デバイスの検出を行う----------------------------------------------------------
		//指定デバイス検出（グローバル変数　a_guid　および　a_id　に注意）
		DWORD deviceCount = 0;
		a_id = id_;
		if(FAILED(  dii->DInput( ).EnumDevices(
							kind[dk].dType,  diEnumDevice,
							&deviceCount, DIEDFL_ATTACHEDONLY))){		return Err::ENUMDEVICE;	}
		if(dk == GamePad  &&  deviceCount <= id_){						return Err::ENUMDEVICE;	}

	//入力デバイスを生成する
		if(FAILED(dii->DInput( ).CreateDevice( a_guid, &device, nullptr))){		return Err::CREATEDEVICE;	}
	//入力デバイスのフォーマットを設定する
		if(FAILED(device->SetDataFormat(kind[dk].dFormat))){					return Err::DATAFORMAT;		}
	//入力デバイスの協調レベルを設定する
		if(FAILED(device->SetCooperativeLevel(dii->Wnd( ), kind[dk].cLevel))){	return Err::COOPLEVEL;		}

	//入力ゲームパッドの場合、方向キーに絶対軸もしくは相対軸が使用可能なこと事を確認する。
		switch(dk)
		{
			case GamePad:
				//絶対軸および相対軸が使用可能な場合、軸の設定を行う
				if(FAILED(device->EnumObjects(	enumObjectsGamePad,
												device,
												DIDFT_AXIS)))
				{
					return Err::GPAXIS;
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
						return Err::KBPROPERTY;
					}
				}			
				break;
		}
		return Err::NOTHING;
	}
	Err::Type Mouse::init(float scaleX_, float scaleY_)
	{
		Obj::SP dii = Obj::GetInst();
		if (dii == 0){ return Err::NOTDEVICE; }

		//--------------------------------
		//DirectMouse
		if (FAILED(dii->DInput().CreateDevice(GUID_SysMouse, &mouse, nullptr))){ return Err::CREATEDEVICE; }
		if (FAILED(mouse->SetDataFormat(&c_dfDIMouse2))){ return Err::DATAFORMAT; } // マウス用のデータ・フォーマットを設定
		mouse->SetCooperativeLevel(dii->Wnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);//レベル設定
		// デバイスの設定    
		DIPROPDWORD diprop;
		diprop.diph.dwSize = sizeof(diprop);
		diprop.diph.dwHeaderSize = sizeof(diprop.diph);
		diprop.diph.dwObj = 0;
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.dwData = DIPROPAXISMODE_REL; // 相対値モードDIPROPAXISMODE_RELで設定（絶対値はDIPROPAXISMODE_ABS）    
		mouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);

		// 入力制御開始    
		mouse->Acquire();
		//--------------------------------
		scaleX = scaleX_;
		scaleY = scaleY_;

		return Err::NOTHING;
	}

	Mouse::SP Mouse::Create(float scaleX_, float scaleY_)
	{
		auto sp = SP(new Mouse());
		if (sp){
			if (sp->init(scaleX_, scaleY_) != Err::NOTHING){
				MessageBox(nullptr, _T("マウスがありませんよ！"), nullptr, MB_OK);
				return nullptr;
			}
		}
		return sp;
	}
	Mouse::Mouse()
	{
	}

	Mouse::~Mouse()
	{
		if (mouse != NULL)
		{
			mouse->Unacquire();
			ML::SafeRelease(&mouse);
		}
	}

	void Mouse::UpDate()
	{
		//--------------------------------
		//add koyanagi
		// 値の更新
		ZeroMemory(&mouseState, sizeof(DIMOUSESTATE2));
		if (FAILED(mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState))){
			mouse->Acquire();
		}
		GetCursorPos(&mouseWPos);
		Obj::SP dii = Obj::GetInst();
		ScreenToClient(dii->Wnd(), &mouseWPos);
		mouseWPos.x = (LONG)(mouseWPos.x / scaleX);
		mouseWPos.y = (LONG)(mouseWPos.y / scaleY);

		mouseCPos.x = mouseState.lX;
		mouseCPos.y = mouseState.lY;
		mouseWheel = mouseState.lZ;
		for (int i = 0; i < MB::kMax; i++)
		{
			if ((mouseState.rgbButtons[i] & 0x80) >> 7){
				if (mouseButton[i] == KS::Off || mouseButton[i] == KS::Up)  mouseButton[i] = KS::Down;
				else  mouseButton[i] = KS::On;
			}
			else{
				if (mouseButton[i] == KS::On || mouseButton[i] == KS::Down)  mouseButton[i] = KS::Up;
				else  mouseButton[i] = KS::Off;
			}
		}
		//--------------------------------
	}
	//==================================
	//Mouseのローカル座標を取得
	//==================================
	POINT& Mouse::Distance()
	{
		return mouseCPos;
	}
	int Mouse::Wheel()
	{
		return mouseWheel;
	}
	//==================================
	//Mouseの絶対座標を取得
	//==================================
	POINT& Mouse::CursorPos()
	{
		return mouseWPos;
	}

	KS::Type Mouse::CheckBT(MB::Type mb)
	{
		return mouseButton[mb];
	}
	bool Mouse::CheckBT(MB::Type mb, KS::Type ks)
	{
		if (CheckBT(mb) == ks)
			return true;
		return false;
	}
	bool  Mouse::Button_On(MB::Type  mb_){ return	 this->CheckBT(mb_, KS::On); }
	bool  Mouse::Button_Off(MB::Type  mb_){ return	 this->CheckBT(mb_, KS::Off); }
	bool  Mouse::Button_Down(MB::Type  mb_){ return  this->CheckBT(mb_, KS::Down); }
	bool  Mouse::Button_Up(MB::Type  mb_){ return	 this->CheckBT(mb_, KS::Up); }
}

namespace DI
{
	DI_::Obj::SP		obj;
	map<string, DI_::Input::SP>  inputs;
	bool inputs_NotObject = false;

	DI_::Mouse::SP  mouse;

	//初期化
	bool  Initialize(HWND		hw_, float scaleX_, float scaleY_)
	{
		if (!obj)
		{
			obj = DI_::Obj::Create(hw_);
			mouse = DI_::Mouse::Create(scaleX_, scaleY_);
		}
		return  obj != nullptr;
	}
	//解放
	void  Finalize()
	{
		GPad_Clear();
		obj.reset();
	}
	//更新
	void  UpDate()
	{
		for (auto it = inputs.begin(); it != inputs.end(); it++)
		{
			if ((*it).second) {
				(*it).second->UpDate();
			}
		}
		mouse->UpDate();
	}
	//現在登録してあるすべてのデバイスを破棄
	void  GPad_Clear()
	{
		inputs.clear();
	}
	//指定したデバイスを破棄
	void  GPad_Erase(const string name_)
	{
		inputs.erase(name_);
	}

	//キーボードでデバイス生成
	void  GPad_CreateKB(
		const string		name_,
		const AnalogAxisKB	axisL_,
		const AnalogAxisKB	axisR_,
		const KeyDatas_KB	button_)
	{
		DI_::AnalogAxisKB  aL;
		memcpy(&aL, &axisL_, sizeof(aL));
		DI_::AnalogAxisKB  aR;
		memcpy(&aR, &axisR_, sizeof(aR));
		auto  bt = new DI_::KeyDataKB[button_.size( )];
		memcpy(bt, &button_[0], sizeof(DI_::KeyDataKB) * button_.size());


		auto  w = DI_::Input::CreateKB(aL, aR, button_.size(), bt);
		delete[] bt;
		if (!w) { return; }
		inputs[name_] = w;
	}
	//ゲームパッドでデバイス生成
	void  GPad_CreateGP(
		const string			name_,
		const DWORD				id_,
		const KeyDatas_GP		button_)
	{
		auto  bt = new DI_::KeyDataGP[button_.size()];
		memcpy(bt, &button_[0], sizeof(DI_::KeyDataGP) * button_.size());

		auto  w = DI_::Input::CreateGP(id_, button_.size(), bt);
		delete[] bt;
		if (!w) { return; }
		inputs[name_] = w;
	}
	//キーボード＆ゲームパッドでデバイス生成
	void  GPad_CreateKBGP(
		const string	name_,
		const AnalogAxisKB	axisL_,
		const AnalogAxisKB	axisR_,
		const KeyDatas_KB	buttonK_,
		const DWORD			id_,
		const KeyDatas_GP	buttonG_)
	{
		DI_::AnalogAxisKB  aL;
		memcpy(&aL, &axisL_, sizeof(aL));
		DI_::AnalogAxisKB  aR;
		memcpy(&aR, &axisR_, sizeof(aR));
		auto  btK = new DI_::KeyDataKB[buttonK_.size()];
		memcpy(btK, &buttonK_[0], sizeof(DI_::KeyDataKB) * buttonK_.size());

		auto  btG = new DI_::KeyDataGP[buttonG_.size()];
		memcpy(btG, &buttonG_[0], sizeof(DI_::KeyDataGP) * buttonG_.size());

		auto  w1 = DI_::Input::CreateKB(aL, aR, buttonK_.size(), btK);
		delete[] btK;
		auto  w2 = DI_::Input::CreateGP(id_, buttonG_.size(), btG);
		delete[] btG;
		w1 = DI_::Input::Link(w1, w2);
		inputs[name_] = w1;
	}

	ButtonInfo ConvertKS(DI_::KS::Type k_)
	{
		ButtonInfo  rtv;
		rtv.down = k_ == DI_::KS::Down;
		rtv.up   = k_ == DI_::KS::Up;
		rtv.off  = k_ == DI_::KS::Off;
		rtv.on   = k_ == DI_::KS::On;
		return  rtv;
	}
	//	ボタンの状態を確認
	VGamePad  GPad_GetState(const string  name_)
	{
		VGamePad  rtv;
		auto it = inputs.find(name_);
		if (it == inputs.end()) {
			if (inputs_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("その名前の仮想コントローラーは存在しません"), MB_OK);
			}
			inputs_NotObject = true;
			
		}
		else {
			auto ls = it->second->StickL();
			rtv.LStick.angle = ls.angle;
			rtv.LStick.volume = ls.volume;
			rtv.LStick.axis = ls.axis;
			rtv.LStick.L = ConvertKS(ls.xm);
			rtv.LStick.R = ConvertKS(ls.xp);
			rtv.LStick.U = ConvertKS(ls.ym);
			rtv.LStick.D = ConvertKS(ls.yp);

			auto rs = it->second->StickR();
			rtv.RStick.angle = rs.angle;
			rtv.RStick.volume = rs.volume;
			rtv.RStick.axis = rs.axis;
			rtv.RStick.L = ConvertKS(rs.xm);
			rtv.RStick.R = ConvertKS(rs.xp);
			rtv.RStick.U = ConvertKS(rs.ym);
			rtv.RStick.D = ConvertKS(rs.yp);

			rtv.B1 = ConvertKS(it->second->CheckBT(DI_::But::B1));
			rtv.B2 = ConvertKS(it->second->CheckBT(DI_::But::B2));
			rtv.B3 = ConvertKS(it->second->CheckBT(DI_::But::B3));
			rtv.B4 = ConvertKS(it->second->CheckBT(DI_::But::B4));

			rtv.L1 = ConvertKS(it->second->CheckBT(DI_::But::L1));
			rtv.L2 = ConvertKS(it->second->CheckBT(DI_::But::L2));
			rtv.L3 = ConvertKS(it->second->CheckBT(DI_::But::L3));

			rtv.R1 = ConvertKS(it->second->CheckBT(DI_::But::R1));
			rtv.R2 = ConvertKS(it->second->CheckBT(DI_::But::R2));
			rtv.R3 = ConvertKS(it->second->CheckBT(DI_::But::R3));

			rtv.ST = ConvertKS(it->second->CheckBT(DI_::But::ST));
			rtv.SE = ConvertKS(it->second->CheckBT(DI_::But::SE));

			rtv.HL = ConvertKS(it->second->CheckBT(DI_::But::L));
			rtv.HR = ConvertKS(it->second->CheckBT(DI_::But::R));
			rtv.HU = ConvertKS(it->second->CheckBT(DI_::But::U));
			rtv.HD = ConvertKS(it->second->CheckBT(DI_::But::D));

			rtv.S0 = ConvertKS(it->second->CheckBT(DI_::But::S0));
			rtv.S1 = ConvertKS(it->second->CheckBT(DI_::But::S1));
			rtv.S2 = ConvertKS(it->second->CheckBT(DI_::But::S2));
			rtv.S3 = ConvertKS(it->second->CheckBT(DI_::But::S3));
			rtv.S4 = ConvertKS(it->second->CheckBT(DI_::But::S4));
			rtv.S5 = ConvertKS(it->second->CheckBT(DI_::But::S5));
			rtv.S6 = ConvertKS(it->second->CheckBT(DI_::But::S6));
			rtv.S7 = ConvertKS(it->second->CheckBT(DI_::But::S7));
			rtv.S8 = ConvertKS(it->second->CheckBT(DI_::But::S8));
			rtv.S9 = ConvertKS(it->second->CheckBT(DI_::But::S9));
		}
		return rtv;
	}
	//	マウスの状態を確認
	Mouse  Mouse_GetState( )
	{
		Mouse  rtv;
		rtv.distance = mouse->Distance();
		rtv.cursorPos = mouse->CursorPos();
		rtv.wheel = mouse->Wheel();
		rtv.LB = ConvertKS(mouse->CheckBT(DI_::MB::LB));
		rtv.RB = ConvertKS(mouse->CheckBT(DI_::MB::RB));
		rtv.CB = ConvertKS(mouse->CheckBT(DI_::MB::CB));

		rtv.B1 = ConvertKS(mouse->CheckBT(DI_::MB::RESERVED1));
		rtv.B2 = ConvertKS(mouse->CheckBT(DI_::MB::RESERVED2));
		rtv.B3 = ConvertKS(mouse->CheckBT(DI_::MB::RESERVED3));
		rtv.B4 = ConvertKS(mouse->CheckBT(DI_::MB::RESERVED4));
		rtv.B5 = ConvertKS(mouse->CheckBT(DI_::MB::RESERVED5));
		return rtv;
	}
}