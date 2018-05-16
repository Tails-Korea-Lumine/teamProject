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
//ゲームエンジンに追加したものの初期化と開放
	bool Init(HWND wnd_);
	~MyGameEngine( );
//ゲームエンジンに追加したもののステップ処理
	void Step( );

//3DPG1対応により追加
	//2D描画環境のデフォルトパラメータ設定
	void Set2DDrawState( );
	//3D描画環境のデフォルトパラメータ設定
	void Set3DDrawState( );
//ゲームエンジンに追加したいものは下に加える
//----------------------------------------------
	Mouse			mouse;		//	マウス
	DI::cInput::SP	in1;		//	入力機器
	POINT			offset;		//	２Ｄ描画時のスクロール量

	DG::cCamera::SP camera;		//	カメラ
	D3DLIGHT9		light;		//	ライト

	HWND			wh;
	tstring			sobPath;
//----------------------------------------------
};

extern MyGameEngine* ge;
