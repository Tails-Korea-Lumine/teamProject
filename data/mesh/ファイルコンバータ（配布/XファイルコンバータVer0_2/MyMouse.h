#pragma once
#include <windows.h>

class Mouse{

	enum ButState{
		Non,		//	マウスボタンは押されていない
		Click,		//	マウスボタンはクリックされた
		On,			//	マウスボタンは押し続けられている
		Off,		//	マウスボタンは離された直後である
	};
	POINT pos;
	bool wml;
	bool wmr;
	bool l;	//左ボタン
	bool r;	//右ボタン
	bool pr_l;	//左ボタン(１フレーム前の状態）
	bool pr_r;	//右ボタン(１フレーム前の状態）

public:
	Mouse( );
	~Mouse( );
	void Check( );
	ButState LBStatus( );
	ButState RBStatus( );
	POINT Pos( );

	void LBStatus(bool s);
	void RBStatus(bool s);
	void Pos(POINT& p);
};
