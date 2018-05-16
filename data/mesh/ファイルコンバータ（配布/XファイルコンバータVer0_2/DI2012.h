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

//┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//┃画像制御系機能群																		　┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
namespace DI
{
	//例外として投げられるエラー情報
	enum eErr
	{
		ERR_NOTHING,		//エラーなし
		ERR_CREATE,			//生成失敗
		ERR_NOTDEVICE,		//デバイスが取得的ない
		ERR_UNNONTYPE,		//不明なデバイス種類
		ERR_ENUMDEVICE,		//デバイスの列挙に失敗
		ERR_CREATEDEVICE,	//デバイスの生成失敗
		ERR_DATAFORMAT,		//データフォーマットの設定失敗
		ERR_COOPLEVEL,		//協調レベル設定失敗
		ERR_GPAXIS,			//
		ERR_KBPROPERTY,
	};
	enum KS{	//キー状態	
		Err, 
		On,		//押している間中
		Off,	//離している間中
		Down,	//押した直後（１フレームのみ反応）
		Up,		//離した直後（１フレームのみ反応）
	};
	enum KK{	//プログラム上で使用できるキーの種類
		X_M, X_P, Y_M, Y_P,	//	４方向キー
		B00, B01, B02, B03, B04, B05, B06, B07,
		B08, B09, B10, B11, B12, B13, B14, B15,
		KMax		//　ボタン総数
	};

	namespace GPK	//ゲームパッドのキーを表す
	{
		enum Type{		B00, B01, B02, B03, B04, B05, B06, B07,
						B08, B09, B10, B11, B12, B13, B14, B15,
						Y_M, Y_P, X_M, X_P,	//	４方向キー
						KMax		//　ボタン総数
		};
	}

	class cObj
	{
		LPDIRECTINPUT8			di;
		HWND					wnd;

		typedef weak_ptr<cObj> WP;
		static cObj::WP		winst;		//	１つしか存在できないインスタンスのウィークポインタ
//		cObj( ){ }
		cObj(HWND wnd_);	
	public:
		typedef shared_ptr<cObj> SP;
		~cObj( );
		//	DIライブラリを生成する
		static cObj::SP Create(HWND wnd_);
		static cObj::SP GetInst( );
		HWND Wnd( ){	return wnd;}
		LPDIRECTINPUT8 Object( ){	return di;}
	};

	struct sKey{
		DWORD	vKind;		//実際のボタンまたはキーボードのバーチャルキー
		KK		kKind;		//ボタンの種類（ＰＧ上での名称）
	};
	class cInput
	{
		enum DeviceKind{	KeyBoard, GamePad, UnNon	};	//デバイスの種類
		struct sKeyParam{
			DWORD		vKind;		//実際のボタンまたはキーボードのバーチャルキー
			KK			kKind;		//ボタンの種類（ＰＧ上での名称）
			DWORD		kState;		//キーの状態
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
	//	デバイスを生成する
		static SP CreateKB(	DWORD				numKey_,
							sKey				kpa_[ ]);
		static SP CreateGP(	DWORD				id_,
							DWORD				numKey_,
							sKey				kpa_[ ]);
	//	更新
		void UpDate( );
	//	ボタンの状態を確認
		bool Check( KK		kk_,
					KS		ks_);	//検出するボタンの状態
	private:
		static SP Create(	DeviceKind			dk_,
							DWORD				id_,
							DWORD				numKey_,
							sKey				kpa_[ ]);
		SP link;
	public:
		//２つのデバイスを統合する
		//安全性を考えるなら、クラスメソッドのLinkを使う事
		SP Link(SP l){ 
			if(this == l.get( )){	return 0;	}
			else{					SP w =link; link = l; return link; }
		}
		//２つのデバイスを統合する
		//片方のデバイスが生成できていない場合の対応あり
		static cInput::SP Link(SP a, SP b);
	};
}

