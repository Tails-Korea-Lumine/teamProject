#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//タイトル画面用タスク
//-----------------------------------------------------------------------------
#include "MyPG.h"

class Title : public BTask
{
	Title( );
public:
	static void ResourceLoad( );
	static void ResourceRelease( );
	static BTask::SP Create( ){	return BTask::SP( new Title( ) );}
	void Step( );
	void Draw2D( );
	void Draw3D( );
	~Title( );
//-----------------------------------------------------------------------------
//タスク内変数の宣言（共有）
//-----------------------------------------------------------------------------
	static int refCnt;//消すな
	static DG::cImage::SP	img;
//-----------------------------------------------------------------------------
//タスク内変数の宣言（個別）
//-----------------------------------------------------------------------------
	int				cnt;
};
