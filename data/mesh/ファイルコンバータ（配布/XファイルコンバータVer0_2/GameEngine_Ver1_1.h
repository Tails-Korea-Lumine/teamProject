#pragma once
#include <windows.h>
#include <functional>
#pragma	comment(lib,"winmm")	//	マルチメディア拡張機能を使用するために必要
#include "DI2012.h"
#include "DG2014.h"
#include "myLib.h"
#include <vector>
#include <map>

//タスク基本クラス
class BTask
{
	static DWORD uKeyManager;	//ユニークキーマネージャ
public:
	enum State{	eActive,	//通常
				eStop,		//停止（ステップ実行停止）
				eKill,		//消滅
				eSuspend,	//サスペンド（一時無効）
				eStateNon = 0xFFFFFFFF};
	DWORD		uKey;			//タスク毎に割り当てられるユニークキー
	tstring		name;			//タスクに割り当てる名前（検索に使用）
	State		state;			//タスクの状態
	float		drawPriority;	//２Ｄ用プライオリティ
	float		drawPriority3D;	//3D用プライオリティ
	bool		alpha;			//3D用プライオリティ

	BTask( ){ }
	BTask(tstring n_, State s_=eActive):name(n_), state(s_), drawPriority(0)
	,drawPriority3D(0), alpha(false){ uKey = uKeyManager++; }
public:
	virtual ~BTask( ){ };
	void T_Step( ){		if(state == eActive){ Step( );}	}
	void T_Draw2D( ){	if(state == eActive || state == eStop){ Draw2D( );}	}
	void T_Draw3D( ){	if(state == eActive || state == eStop){ Draw3D( );}	}
	virtual void Step( )=0;
	virtual void Draw2D( )=0;
	virtual void Draw3D( )=0;
	typedef shared_ptr<BTask>	SP;
	typedef weak_ptr<BTask>		WP;
	void Kill( ){		state = eKill;	}							//消滅させる
	void Suspend( ){	if(state != eKill){ state = eSuspend;	} }	//サスペンドにする
	void Active( ){		if(state != eKill){ state = eActive;	} }	//アクティブに戻す
	void Stop( ){		if(state != eKill){ state = eStop;		} }	//停止状態にする
	bool KillCheck( ){	return (state == eKill); }					//消滅状態か否かを確認
	tstring& Name( ){return name;}
	float DrawPriority( ){return drawPriority;}
};

//	ゲームエンジン
class GameEngine
{
public:
	DWORD			screenWidth;		//	画面サイズ
	DWORD			screenHeight;		//	画面サイズ
	DWORD			viewScale;			//	表示倍率
	bool			screenMode;			//	フルスクリーンモード
	char*			windowTitle;		//	ウインドウタイトル
	char*			windowClassName;	//	ウィンドウクラス名
	bool			quitRequest;		//	GameMainに終了要請する時trueにする
	DG::cObj::SP	dg;					//	グラフィックライブラリ
	DI::cObj::SP	di;					//	入力ライブラリ
	DWORD			bgColor;			//	背景色

private:
	vector<BTask::SP>  tasks;
	vector<BTask::SP>  addtasks;
public:
	GameEngine(
		DWORD		sw_,	//	画面サイズ
		DWORD		sh_,	//	画面サイズ
		DWORD		vs_,	//	表示倍率
		bool		sm_,	//	フルスクリーンモード
		TCHAR*		wtn_,	//	ウインドウタイトル
		TCHAR*		wcn_)	//	ウィンドウクラス名
	:screenWidth(sw_)
	,screenHeight(sh_)
	,viewScale(vs_)
	,screenMode(sm_)
	,windowTitle(wtn_)
	,windowClassName(wcn_)
	,quitRequest(false)
	,bgColor(0x00FFFFFF)
	{ }
	//	ゲーム処理
	void B_Step(HWND wnd_);
	//	初期化
	bool B_Init(HWND wnd_);
	//	解放
	virtual ~GameEngine( );
	//	タスクの登録
	void PushBack(const BTask::SP& t_); 
	//	同名のタスクを全て削除する
	bool KillAll(const tstring& k_);
	//	同名のタスクを全て停止する
	bool StopAll(const tstring& k_);
	//	同名のタスクを全て一時無効にする
	bool SuspendAll(const tstring& k_);

	//┌───────────────────────────────────────────┐
	//｜同名対象の中から指定番号目を検出													  ｜
	//｜履歴		：2013/02/27	須賀康之	作成										　｜
	//└───────────────────────────────────────────┘
	template <class T>
	shared_ptr<T> GetTask(const tstring& k_, DWORD n_)
	{
		for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
			if((*it)->KillCheck( ) == false){
				if((*it)->Name( ) == k_){
					if(n_ == 0){
						return static_pointer_cast<T>(*it);
					}
					else{--n_;}
				}
			}
		}
		shared_ptr<T> rtv;
		return  rtv;
	}
	virtual bool Init(HWND wnd_)=0;
	virtual void Step( )=0;

	//3DPG1対応により追加
	//2D描画環境のデフォルトパラメータ設定
	virtual void Set2DDrawState( )=0;
	//3D描画環境のデフォルトパラメータ設定
	virtual void Set3DDrawState( )=0;

	//┌───────────────────────────────────────────┐
	//｜指定名（複数指定可）の対象を検出し、そのベクターを返す								  ｜
	//｜履歴		：2013/03/21	須賀康之	作成										　｜
	//└───────────────────────────────────────────┘
	template <class T>
	bool GetTasks(vector<shared_ptr<T>>& tasks_,  const vector<tstring> tgNames_, DWORD exclusionKey_ = 0)
	{
		bool rtv = false;
		
		//名前の一覧ループ
		for(auto itN = tgNames_.begin( ); itN != tgNames_.end( ); ++itN){
			//タスクの一覧ループ
			for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
				if((*it)->KillCheck( ) == false){
					if((*it)->Name( ) == (*itN)){
						if(exclusionKey_ != (*it)->uKey){
							tasks_.push_back( static_pointer_cast<T>(*it) );
							rtv = true;
						}
					}
				}
			}
		}
		return rtv;
	}

};



