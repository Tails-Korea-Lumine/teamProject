#include "MyPG.h"
#include "Task_Title.h"

namespace MyPG
{
	

	MyGameEngine::MyGameEngine( )
	:GameEngine(
	//	環境情報
		960,							//	画面サイズX
		540,							//	画面サイズY
		1,								//	表示倍率
		1,								//	マルチサンプル(1or2or4)
		false,							//	フルスクリーンモード
		480,							//	2D描画用解像度X(現在非対応）
		270,							//	2D描画用解像度Y(現在非対応）
		"３Ｄゲームプログラミング",		//	ウインドウタイトル
		"3D_GPG"						//	ウィンドウクラス名
	){
	}

	//ゲームエンジンに追加したものの初期化と開放
	bool MyGameEngine::Initialize(HWND wnd_)
	{
		//　画像描画ライブラリの初期化
		{
			bool  rtv = DG::Initialize(	wnd_, this->screenWidth,  this->screenHeight,  this->multiSample,
										this->screenMode, this->screen2DWidth, this->screen2DHeight);
			if (!rtv){
				//	環境の初期化に失敗
				DestroyWindow(wnd_);
				return false;
			}
		}
		//	入力機器ライブラリの初期化
		{
			bool  rtv = DI::Initialize(wnd_, this->viewScaleW, this->viewScaleH);
			if (!rtv){
				//	環境の初期化に失敗
				DestroyWindow(wnd_);
				return false;
			}
		}
		//デバッグ用フォントの準備
		DG::Font_Create("Debug", "ＭＳ Ｐゴシック", 4, 16);

		//デフォルトカメラ
		ge->camera[0] = MyPG::Camera::Create(
			D3DXVECTOR3( 1.0f, 0.0f, 0.0f),	//	ターゲット位置
			D3DXVECTOR3( 0.0f, 0.0f, 0.0f),	//	カメラ位置
			D3DXVECTOR3(0.0f, 1.0f, 0.0f),		//	カメラの上方向ベクトル
			ML::ToRadian(60.0f), 10.0f, 300.0f,	//	視野角・視野距離
			(float)ge->screenWidth / (float)ge->screenHeight);		//	画面比率
		DG::EffectState( ).param.bgColor  = ML::Color(1,1,1,1);
		ge->camera[3] = ge->camera[2] = ge->camera[1] = ge->camera[0];

		//デバッグ用情報の表示ON/OFF
		DebugMode(false);

		//キーボードとゲームパッドの入力を受け取るオブジェクトを生成する
		//キーボードパラメータ
		DI::AnalogAxisKB	ls = { DIK_LEFT, DIK_RIGHT, DIK_UP, DIK_DOWN };
		DI::AnalogAxisKB	rs = { DIK_NUMPAD4, DIK_NUMPAD6, DIK_NUMPAD8, DIK_NUMPAD2 };
		DI::KeyDatas_KB	 key = {
			{ DIK_Z, DI::But::B1 },{ DIK_X, DI::But::B2 },{ DIK_C, DI::But::B3 },
			{ DIK_V, DI::But::B4 },{ DIK_A, DI::But::SE },{ DIK_S, DI::But::ST },
			{ DIK_Q, DI::But::L1 },{ DIK_W, DI::But::R1 },{ DIK_E, DI::But::L2 },
			{ DIK_R, DI::But::R2 },{ DIK_D, DI::But::L3 },{ DIK_NUMPAD5, DI::But::R3 },
		};
		//ゲームパッドパラメータ
		DI::KeyDatas_GP	but = {
			{ DI::GPB::B01, DI::But::B1 },{ DI::GPB::B02, DI::But::B2 },
			{ DI::GPB::B03, DI::But::B3 },{ DI::GPB::B04, DI::But::B4 },
			{ DI::GPB::B05, DI::But::L1 },{ DI::GPB::B06, DI::But::R1 },
			{ DI::GPB::B07, DI::But::L2 },{ DI::GPB::B08, DI::But::R2 },
			{ DI::GPB::B09, DI::But::SE },{ DI::GPB::B10, DI::But::ST },
			{ DI::GPB::B11, DI::But::L3 },{ DI::GPB::B12, DI::But::R3 },
		};
		DI::GPad_CreateKBGP("P1", ls, rs, key, 0, but);


		//背景色の設定（デフォルト）
		DG::EffectState( ).param.bgColor  =  ML::Color(0,0.0f,0.8f,0.0f);

		//ビューポートの設定（デフォルト)
		D3D10_VIEWPORT defVP = { 0,0,this->screenWidth,this->screenHeight,0.0f,1.0f }; //X Y W H MIND MAXD

		this->vp3D_L0 = defVP;
		this->vp3D_L1 = defVP;
		this->vp3D_L2 = defVP;
		this->vp3D_L3 = defVP;
		this->vp3D_AF = defVP;
		this->vp3D_BF = defVP;

		//初期実行タスク生成＆ゲームエンジンに登録
		auto  firstTask = Title::Object::Create(true);

//------------------------------------------------------------------------------------
//レイヤー毎の描画のON/OFF
//------------------------------------------------------------------------------------
		Layer_3D(true, false, false, false);
		Layer_2D(false, true);
		return true;
	}
	MyGameEngine::~MyGameEngine( )
	{
	//	入力装置の解放
		DI::GPad_Erase("P1");
	}
	//ゲームエンジンに追加したもののステップ処理
	void MyGameEngine::UpDate( )
	{
	//	入力装置の更新
		DI::UpDate();
	////	サウンドの更新(ストリーミング監視）
	//	dm->Check_Stream( );
	}

	//2D描画環境のデフォルトパラメータ設定
	void MyGameEngine::Set2DRenderState(DWORD l_)
	{
		//共通？
		DG::EffectState( ).RS_Def2D( );
		DG::EffectState().BS_Alpha();
		if (l_ == 0)
		{
			DG::EffectState().ViewPort(vp3D_BF);
		}
		if (l_ == 1)
		{
			DG::EffectState().ViewPort(vp3D_AF);
		}
	}
	//3D描画環境のデフォルトパラメータ設定
	void MyGameEngine::Set3DRenderState(DWORD l_)
	{
		//出力マージャを初期状態に戻す
		DG::EffectState( ).BS_Std();
		//	カメラの設定更新
		camera[l_]->UpDate( );
		//レイヤー0番に対する設定
		if (l_ == 0)
		{
			DG::EffectState().ViewPort(vp3D_L0);
		}
		if (l_ == 1)
		{
			DG::EffectState().ViewPort(vp3D_L1);
		}
		if (l_ == 2)
		{
			DG::EffectState().ViewPort(vp3D_L2);
		}
		if (l_ == 3)
		{
			DG::EffectState().ViewPort(vp3D_L3);
		}
	}
	//------------------------------------------------------------
	Camera::Camera(		const ML::Vec3&  tg_,	//	被写体の位置
						const ML::Vec3&  pos_,	//	カメラの位置
						const ML::Vec3&  up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
						float            fov_,	//	視野角
						float            np_,	//	前クリップ平面（これより前は映らない）
						float            fp_,	//	後クリップ平面（これより後ろは映らない）
						float            asp_)	//	アスペクト比（画面の比率に合わせる　横÷縦）	
	: pos(pos_), target(tg_), up(up_), fov(fov_), nearPlane(np_), forePlane(fp_), aspect(asp_)
	{
		UpDate( );
	}
	Camera::~Camera( )
	{
	}
	//	カメラを生成する
	Camera::SP Camera::Create(	const ML::Vec3&  tg_,	//	被写体の位置
								const ML::Vec3&  pos_,	//	カメラの位置
								const ML::Vec3&  up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
								float            fov_,	//	視野角
								float            np_,	//	前クリップ平面（これより前は映らない）
								float            fp_,	//	後クリップ平面（これより後ろは映らない）
								float            asp_)	//	アスペクト比（画面の比率に合わせる　横÷縦）	
	{
		return Camera::SP( new Camera(tg_, pos_, up_, fov_, np_, fp_, asp_) );
	}
	//	カメラの設定
	void Camera::UpDate( )
	{
		auto  dgi = DG_::DGObject::GetInst( );	if(dgi == 0){	return; }

	//ビュー行列を設定
		matView.LookAtLH(pos, target, up);
	//プロジェクション行列を設定
		matProj.PerspectiveFovLH(fov, aspect, nearPlane, forePlane);
	//カメラ関係の更新
		DG::EffectState( ).param.matView			=  matView;
		DG::EffectState( ).param.matProjection	=  matProj;
		DG::EffectState( ).param.eyePos			=  pos;
	}
}

MyPG::MyGameEngine* ge;
