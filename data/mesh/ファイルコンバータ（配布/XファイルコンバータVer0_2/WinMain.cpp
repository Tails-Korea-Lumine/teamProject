#pragma	comment(lib,"winmm")	//	マルチメディア拡張機能を使用するために必要
#include "MyPG.h"
#include "Task_Player.h"

LRESULT CALLBACK WndProc(HWND wnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_);
HWND MyPG_CreateWindow(HINSTANCE inst_, char cName_[], char tName_[], RECT *wSize_, int showCmd_);

//-----------------------------------------------------------------------------
//ウィンメイン
//機能概要：ウィンドウズプログラムのメイン関数
//-----------------------------------------------------------------------------
int __stdcall WinMain(	HINSTANCE inst_,	//
						HINSTANCE,			//
						LPSTR,				//
						int showCmd)		//
{
	ge = new MyGameEngine( );

	MSG		msg;
	HWND	wnd;								//	ウインドウハンドル
	RECT	ws = ML::Rect(0, 0, 
		(int)(ge->screenWidth * ge->viewScale),
		(int)(ge->screenHeight * ge->viewScale));
	wnd = MyPG_CreateWindow(inst_, ge->windowClassName, ge->windowTitle, &ws, showCmd);
	if(wnd == NULL){ return 0;}

//	ゲーム環境の初期化
	ge->B_Init(wnd);

//	メッセージループ
	while(1)
	{
	//	メッセージが来ているか
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
		//	メッセージの列挙が終わった
			if(!GetMessage(&msg, NULL, 0, 0))
			{
				break;
			}
		//	メッセージに応じた処理を行う
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(GetActiveWindow() == wnd)
		{
			ge->B_Step(wnd);		//	ゲーム処理
		}
	}

//	ゲーム環境の破棄
	//ge->~GameEngine( );
	return 0;
}

//-----------------------------------------------------------------------------
//ウィンドウ生成
//機能概要：ウィンドウを作るための各種設定を行い、ウィンドウを生成する
//-----------------------------------------------------------------------------
HWND MyPG_CreateWindow(HINSTANCE inst_, char cName_[], char tName_[], RECT* wSize_, int showCmd_)
{
	WNDCLASSEX wcex;						//	ウインドウ作成に使用
	HWND wnd;								//	ウインドウハンドル

//	データ初期化
	wcex.style			= (CS_HREDRAW | CS_VREDRAW);
	wcex.hIcon			= LoadIcon(inst_, IDI_APPLICATION);
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hIconSm		= LoadIcon(inst_, IDI_APPLICATION);
	wcex.hInstance		= inst_;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= cName_;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
//	Windowsに登録
	if(!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, _T("ウインドウ登録に失敗"), NULL, MB_OK);
		return	NULL;
	}
//	ウインドウ作成
	AdjustWindowRectEx(wSize_, (WS_CAPTION | WS_SYSMENU), NULL, WS_EX_APPWINDOW);

	wnd = CreateWindowEx(	WS_EX_APPWINDOW | WS_EX_ACCEPTFILES, 
							cName_,
							tName_,
							(WS_CAPTION | WS_SYSMENU), 0, 0,
							(wSize_->right  - wSize_->left),
							(wSize_->bottom - wSize_->top),
							NULL, NULL, inst_, NULL);
	if(!wnd)
	{
		MessageBox(NULL, _T("ウインドウ生成に失敗"), NULL, MB_OK);
		return	NULL;
	}

//	ウインドウの状態を更新する
	ShowWindow(wnd, showCmd_);
	UpdateWindow(wnd);

	return wnd;	//成功
}

//-----------------------------------------------------------------------------
//ウィンドウプロシジャ
//機能概要：メッセージに対する処理を行う
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(	HWND	wnd_,		//	ウィンドウハンドル
							UINT	msg_,		//	メッセージ
							WPARAM	wParam_,	//	パラメータ
							LPARAM	lParam_)	//	パラメータ
{
	LRESULT ret = (LRESULT)0;
//	Windowsから送信されたメッセージを調べる
	switch(msg_){

	//	ウインドウが生成された
		case WM_CREATE:
			break;

	//	このウインドウがアクティブになった
		case WM_ACTIVATEAPP:
			break;
	//ファイルがドロップされたよ
		case WM_DROPFILES:
			{
				UINT  bufferSize = DragQueryFileA((HDROP)wParam_, 0, nullptr, 0);
				vector<char>  buffer(bufferSize+10);
				DragQueryFileA((HDROP)wParam_, 0, &buffer[0], bufferSize+10);
				//
				tstring  fpath;
				fpath = &buffer[0];
				ge->sobPath = fpath;
				ge->PushBack( Player::Create(ML::Vec3(50.0f,100.0f,50.0f)) );

				DragFinish((HDROP)wParam_);
			}
			break;

	//	×ボタンが押された
		case WM_CLOSE:
			ge->quitRequest = true;
			break;

	//	ウインドウが破棄された
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

	//	その他のメッセージはWindowsに処理を任せる
		default:
			ret = DefWindowProc(wnd_, msg_, wParam_, lParam_);
	}
	return ret;
}

