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
//┃入力制御系機能群																		　┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
namespace DI
{
	enum But {	//プログラム上で使用できるキーの種類
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
		HL,		//ハットスイッチ
		//→
		HR,
		//↑
		HU,
		//↓
		HD,
		S0, S1, S2, S3, S4, S5, S6, S7, S8, S9,
	};
	enum GPB {//ゲームパッドのキーを表す
		B01, B02, B03, B04, B05, B06, B07, B08,
		B09, B10, B11, B12, B13, B14, B15, B16,
		HSL, HSR, HSU, HSD,		//ハットスイッチ 
	};

	struct  KD_KB{
		DWORD           vKey;	//キーボードのバーチャルキー
		But             pKey;	//ボタンの種類（ＰＧ上での名称）
	};
	//キー設定キーボード用
	typedef  vector<KD_KB>	KeyDatas_KB;
	//アナログスティック設定キーボード用
	struct  AnalogAxisKB{
		DWORD           vKeyL;	//キーボードのバーチャルキー
		DWORD           vKeyR;	//キーボードのバーチャルキー
		DWORD           vKeyU;	//キーボードのバーチャルキー
		DWORD           vKeyD;	//キーボードのバーチャルキー
	};
	struct  KD_GP{
		GPB				 gKey;	//ゲームパッドのボタン
		But              pKey;	//ボタンの種類（ＰＧ上での名称）
	};
	//キー設定ゲームパッド用
	typedef  vector<KD_GP>	KeyDatas_GP;

	struct  ButtonInfo {
		bool  on = false;
		bool  off = true;
		bool  down = false;
		bool  up = false;
		//ボタンをONに書き換える
		void  SetOn() {   on = 1; off = 0; down = 0; up = 0; }
		//ボタンをOFFに書き換える
		void  SetOff() {  on = 0; off = 1; down = 0; up = 0; }
		//ボタンをDOWNに書き換える
		void  SetDown() { on = 1; off = 0; down = 1; up = 0; }
		//ボタンをUPに書き換える
		void  SetUp() {   on = 0; off = 1; down = 0; up = 1; }
	};
	//アナログスティック情報
	struct  StickInfo {
		float			angle = 0.0f;		//アナログスティックの角度（RT0度　DW90度のラジアン）
		float			volume = 0.0f;	//アナログスティックの強弱0.0f～1.0f
		ML::Vec2		axis = ML::Vec2(0,0);		//軸毎の量-1.0f～+1.0f
		ButtonInfo      L;		//ボタンに見立てた状態　左
		ButtonInfo      R;		//ボタンに見立てた状態　右
		ButtonInfo      U;		//ボタンに見立てた状態　上
		ButtonInfo      D;		//ボタンに見立てた状態　下
	};
	//仮想コントローラ構造体
	struct  VGamePad {
		StickInfo  LStick;
		StickInfo  RStick;
		ButtonInfo  B1;
		ButtonInfo  B2;
		ButtonInfo  B3;
		ButtonInfo  B4;
		ButtonInfo  L1;
		ButtonInfo  L2;
		ButtonInfo  L3;
		ButtonInfo  R1;
		ButtonInfo  R2;
		ButtonInfo  R3;
		ButtonInfo  ST;
		ButtonInfo  SE;
		ButtonInfo  HL;
		ButtonInfo  HR;
		ButtonInfo  HU;
		ButtonInfo  HD;
		ButtonInfo  S0;
		ButtonInfo  S1;
		ButtonInfo  S2;
		ButtonInfo  S3;
		ButtonInfo  S4;
		ButtonInfo  S5;
		ButtonInfo  S6;
		ButtonInfo  S7;
		ButtonInfo  S8;
		ButtonInfo  S9;
	};
	//マウス構造体
	struct  Mouse {
		POINT  distance = { 0,0 };	//移動量
		POINT  cursorPos = { 0,0 };	//現在位置
		int    wheel = 0;		//ホイール
		ButtonInfo  LB;		//左ボタン
		ButtonInfo  RB;		//右ボタン
		ButtonInfo  CB;		//中央ボタン（ホイール
		ButtonInfo  B1;		//拡張ボタン
		ButtonInfo  B2;		//拡張ボタン
		ButtonInfo  B3;		//拡張ボタン
		ButtonInfo  B4;		//拡張ボタン
		ButtonInfo  B5;		//拡張ボタン
	};

	//初期化
	bool  Initialize(HWND  hw_, float scaleX_ = 1.f, float scaleY_ = 1.f);
	//解放
	void  Finalize();
	//更新
	void  UpDate();
	//現在登録してあるすべてのデバイスを破棄
	void  GPad_Clear();
	//指定したデバイスを破棄
	void  GPad_Erase(const string name_);



	//キーボードでデバイス生成
	void  GPad_CreateKB(
		const string	name_,
		const AnalogAxisKB	axisL_,
		const AnalogAxisKB	axisR_,
		const KeyDatas_KB		button_);
	//ゲームパッドでデバイス生成
	void  GPad_CreateGP(
		const string	name_,
		const DWORD			id_,
		const KeyDatas_GP		button_);
	//キーボード＆ゲームパッドでデバイス生成
	void  GPad_CreateKBGP(
		const string	name_,
		const AnalogAxisKB	axisL_,
		const AnalogAxisKB	axisR_,
		const KeyDatas_KB		buttonK_,
		const DWORD			id_,
		const KeyDatas_GP		buttonG_);
	//	ボタンの状態を確認
	VGamePad  GPad_GetState(const string  name_);

	//	マウスの状態を確認
	Mouse  Mouse_GetState( );
}