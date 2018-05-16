#include "MyPG.h"
#include "Task_Game.h"

MyGameEngine* ge;

MyGameEngine::MyGameEngine( )
:GameEngine(
//	環境情報
	960,							//	画面サイズX
	540,							//	画面サイズY
	1,								//	表示倍率
	false,							//	フルスクリーンモード
	_T("3D_GPG"),					//	ウインドウタイトル
	_T("３Ｄゲームプログラミング"))	//	ウィンドウクラス名
{
}

//ゲームエンジンに追加したものの初期化と開放
bool MyGameEngine::Init(HWND wnd_)
{
	wh = wnd_;
	//	入力装置の設定
	DI::sKey	k1[10] = {
		//キーボードのキー名
		//↓		プログラム上で使用するボタン名
		//↓		↓
		{DIK_LEFT,	DI::X_M	},	//左
		{DIK_RIGHT,	DI::X_P	},	//右
		{DIK_UP,	DI::Y_M	},	//上
		{DIK_DOWN,	DI::Y_P	},	//下
		{DIK_Z,		DI::B00	},	//00ボタン
		{DIK_X,		DI::B01	},	//01ボタン
		{DIK_C,		DI::B02	},	//02ボタン
		{DIK_V,		DI::B03	},	//03ボタン
		{DIK_B,		DI::B04	},	//04ボタン
		{DIK_N,		DI::B05	},	//05ボタン
	};
	//キーボード入力を受け取るオブジェクトを生成
	in1 = DI::cInput::CreateKB(10, k1);

	//	２Ｄ描画時のスクロール量
	offset.x = 0;
	offset.y = 0;
	//	３Ｄ描画時のカメラ（デフォルト）
	//	カメラの設定
	camera = DG::cCamera::Create(
		D3DXVECTOR3( 500.0f,   0.0f,   400.0f),		//	ターゲット位置
		D3DXVECTOR3( 400.0f, 1200.0f, -800.0f),		//	カメラ位置
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),				//	カメラの上方向ベクトル
		35.0f * D3DX_PI / 180.0f, 10.0f, 10000.0f,	//	視野角・視野距離
		(float)ge->screenWidth / (float)ge->screenHeight);		//	画面比率

	//	ライトの設定（デフォルト）
	D3DLIGHT9 wL;
	ZeroMemory(	&wL, sizeof(wL));
	wL.Type = D3DLIGHT_DIRECTIONAL;				//ディレクショナルライトを使用
	ML::Vec3 vecDir = ML::Vec3(3.0f,  -10.0f, 7.0f);		//照射方向
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

	//初期実行タスク生成
	PushBack( Game::Create( ) );
	return true;
}
MyGameEngine::~MyGameEngine( )
{
//	入力装置の解放
	in1.reset( );
}
//ゲームエンジンに追加したもののステップ処理
void MyGameEngine::Step( )
{
//	入力装置の更新
	in1->UpDate( );
	mouse.Check( );
}

//2D描画環境のデフォルトパラメータ設定
void MyGameEngine::Set2DDrawState( )
{
}
//3D描画環境のデフォルトパラメータ設定
void MyGameEngine::Set3DDrawState( )
{
	//ライトの設定
	dg->Device( )->SetRenderState(D3DRS_LIGHTING, TRUE);
	dg->Device( )->LightEnable(0, TRUE);
	dg->Device( )->SetLight(0, &light);
	//d3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);	//スペキュラ有効

	//アルファ有効
	dg->Device( )->SetRenderState(	D3DRS_ALPHABLENDENABLE,	TRUE);
	//転送元ブレンド係数(Ａ)
	dg->Device( )->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//転送先ブレンド係数(１－Ａ)
	dg->Device( )->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//テクスチャ合成
	dg->Device( )->SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE ); 
	dg->Device( )->SetTextureStageState(0, D3DTSS_COLOROP,		D3DTOP_MODULATE ); 
	dg->Device( )->SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE ); 

	dg->Device( )->SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE ); 
	dg->Device( )->SetTextureStageState(0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE ); 
	dg->Device( )->SetTextureStageState(0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );

	//	カメラの設定更新
	camera->UpDate( );

	D3DVIEWPORT9 vp = {0,0,ge->screenWidth, ge->screenHeight, 0.0f, 1.0f};
	dg->Device( )->SetViewport(&vp);
}