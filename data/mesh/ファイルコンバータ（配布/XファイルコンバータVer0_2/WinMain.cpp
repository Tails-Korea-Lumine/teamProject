#pragma	comment(lib,"winmm")	//	�}���`���f�B�A�g���@�\���g�p���邽�߂ɕK�v
#include "MyPG.h"
#include "Task_Player.h"

LRESULT CALLBACK WndProc(HWND wnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_);
HWND MyPG_CreateWindow(HINSTANCE inst_, char cName_[], char tName_[], RECT *wSize_, int showCmd_);

//-----------------------------------------------------------------------------
//�E�B�����C��
//�@�\�T�v�F�E�B���h�E�Y�v���O�����̃��C���֐�
//-----------------------------------------------------------------------------
int __stdcall WinMain(	HINSTANCE inst_,	//
						HINSTANCE,			//
						LPSTR,				//
						int showCmd)		//
{
	ge = new MyGameEngine( );

	MSG		msg;
	HWND	wnd;								//	�E�C���h�E�n���h��
	RECT	ws = ML::Rect(0, 0, 
		(int)(ge->screenWidth * ge->viewScale),
		(int)(ge->screenHeight * ge->viewScale));
	wnd = MyPG_CreateWindow(inst_, ge->windowClassName, ge->windowTitle, &ws, showCmd);
	if(wnd == NULL){ return 0;}

//	�Q�[�����̏�����
	ge->B_Init(wnd);

//	���b�Z�[�W���[�v
	while(1)
	{
	//	���b�Z�[�W�����Ă��邩
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
		//	���b�Z�[�W�̗񋓂��I�����
			if(!GetMessage(&msg, NULL, 0, 0))
			{
				break;
			}
		//	���b�Z�[�W�ɉ������������s��
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(GetActiveWindow() == wnd)
		{
			ge->B_Step(wnd);		//	�Q�[������
		}
	}

//	�Q�[�����̔j��
	//ge->~GameEngine( );
	return 0;
}

//-----------------------------------------------------------------------------
//�E�B���h�E����
//�@�\�T�v�F�E�B���h�E����邽�߂̊e��ݒ���s���A�E�B���h�E�𐶐�����
//-----------------------------------------------------------------------------
HWND MyPG_CreateWindow(HINSTANCE inst_, char cName_[], char tName_[], RECT* wSize_, int showCmd_)
{
	WNDCLASSEX wcex;						//	�E�C���h�E�쐬�Ɏg�p
	HWND wnd;								//	�E�C���h�E�n���h��

//	�f�[�^������
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
//	Windows�ɓo�^
	if(!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, _T("�E�C���h�E�o�^�Ɏ��s"), NULL, MB_OK);
		return	NULL;
	}
//	�E�C���h�E�쐬
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
		MessageBox(NULL, _T("�E�C���h�E�����Ɏ��s"), NULL, MB_OK);
		return	NULL;
	}

//	�E�C���h�E�̏�Ԃ��X�V����
	ShowWindow(wnd, showCmd_);
	UpdateWindow(wnd);

	return wnd;	//����
}

//-----------------------------------------------------------------------------
//�E�B���h�E�v���V�W��
//�@�\�T�v�F���b�Z�[�W�ɑ΂��鏈�����s��
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(	HWND	wnd_,		//	�E�B���h�E�n���h��
							UINT	msg_,		//	���b�Z�[�W
							WPARAM	wParam_,	//	�p�����[�^
							LPARAM	lParam_)	//	�p�����[�^
{
	LRESULT ret = (LRESULT)0;
//	Windows���瑗�M���ꂽ���b�Z�[�W�𒲂ׂ�
	switch(msg_){

	//	�E�C���h�E���������ꂽ
		case WM_CREATE:
			break;

	//	���̃E�C���h�E���A�N�e�B�u�ɂȂ���
		case WM_ACTIVATEAPP:
			break;
	//�t�@�C�����h���b�v���ꂽ��
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

	//	�~�{�^���������ꂽ
		case WM_CLOSE:
			ge->quitRequest = true;
			break;

	//	�E�C���h�E���j�����ꂽ
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

	//	���̑��̃��b�Z�[�W��Windows�ɏ�����C����
		default:
			ret = DefWindowProc(wnd_, msg_, wParam_, lParam_);
	}
	return ret;
}

