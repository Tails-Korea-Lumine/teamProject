#include "GameEngine_Ver1_1.h"
#include "MyPG.h"
#include <time.h>
DWORD BTask::uKeyManager = 10000;
//┌───────────────────────────────────────────┐
//｜ゲーム環境の初期化																	　｜
//｜履歴		：2013/02/25	須賀康之	作成										　｜
//└───────────────────────────────────────────┘
bool GameEngine::B_Init(HWND wnd_)
{
//	タイマー分解能を1ミリ秒に設定
	timeBeginPeriod(1);
//	画像処理環境の初期化
	dg = DG::cObj::Create(	wnd_,				
							screenWidth,
							screenHeight,
							screenMode);
	if(dg == 0){
	//	環境の初期化に失敗
		DestroyWindow(wnd_);
		return false;
	}
//	入力機器環境の初期化
	di = DI::cObj::Create(	wnd_);
	if(di == 0){
	//	環境の初期化に失敗
		DestroyWindow(wnd_);
		return false;
	}

	Init(wnd_);
	return true;
}
//┌───────────────────────────────────────────┐
//｜ゲーム処理																			　｜
//｜履歴		：2013/02/25	須賀康之	作成										　｜
//└───────────────────────────────────────────┘
class cKillCheck{
public:
	// ステートがeKillの時trueを返す
	bool operator()(BTask::SP t_) const { return t_->KillCheck( ); }
};
void GameEngine::B_Step(HWND wnd_)
{
//	MyPG_Execute( );//スケルトン２移行により削除
//	ESCキーが押された	終了要請フラグをＯＮ
	if(	GetAsyncKeyState(VK_ESCAPE)){	quitRequest = true;}

//	タスクのステップ処理
	Step( );
	for_each(tasks.begin( ), tasks.end( ), mem_fn(&BTask::T_Step));
//	追加されたタスクの登録
	for(auto it = addtasks.begin( ); it != addtasks.end( ); ++it){
		tasks.push_back((*it));
	}
	addtasks.erase(addtasks.begin( ), addtasks.end( ));

	Set3DDrawState( );
	dg->Begin( bgColor );
//	タスクの描画処理(3D)
	typedef std::pair<float, BTask::SP> dmap;
	multimap<float, BTask::SP> draw3DMM;
	multimap<float, BTask::SP> draw3DAPMM;
	float c=0;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		if(!(*it)->alpha){
			//透過なし：手前から奥に並べる
			draw3DMM.insert( dmap( (*it)->drawPriority3D, (*it) ) );
		}else{
			//透過あり：奥から手前に並べる
			draw3DAPMM.insert( dmap( -(*it)->drawPriority3D, (*it) ) );
		}
		c+=1.0f;
	}
	dg->Begin3D( );
	//透過なしオブジェクトの描画
	for(auto it = draw3DMM.begin( ); it != draw3DMM.end( ); ++it){		(*it).second->T_Draw3D( );	}
	//透過ありオブジェクトの描画
	for(auto it = draw3DAPMM.begin( ); it != draw3DAPMM.end( ); ++it){	(*it).second->T_Draw3D( );	}
	dg->End3D( );


//	タスクの描画処理(2D)
	typedef std::pair<float, BTask::SP> dmap;
	multimap<float, BTask::SP> draw2DMM;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		draw2DMM.insert( dmap( -(*it)->drawPriority, (*it) ) );
	}
	dg->Begin2D( );
	Set2DDrawState( );
	//for_each(draw2DMM.begin( ), draw2DMM.end( ), mem_fn(&BTask::Draw2D));
	for(auto it = draw2DMM.begin( ); it != draw2DMM.end( ); ++it){
		(*it).second->T_Draw2D( );
	}
	dg->End2D( );
	dg->End( );

//	消滅状態のタスクを取り除く
	auto endIt = remove_if(tasks.begin( ), tasks.end( ), cKillCheck( ) );	//Kill状態をリスト後方へ
	tasks.erase(endIt, tasks.end( ));	//Kill状態の全てを削除する

//	タスクが存在しない場合終了とする
//	終了要請がある時　ウィンドウの破棄を行う
	if(	!tasks.size( ) ){			DestroyWindow(wnd_);	}
	else if(quitRequest == true){
		tasks.erase(tasks.begin( ), tasks.end( ));	//残存する全てのタスクを削除する
		DestroyWindow(wnd_);
	}
}
//┌───────────────────────────────────────────┐
//｜ゲーム環境の解放																	　｜
//｜履歴		：2010/02/26	須賀康之	作成										　｜
//└───────────────────────────────────────────┘
GameEngine::~GameEngine( )
{
//	タイマー分解能の設定（1ミリ秒単位）を解除する
	timeEndPeriod(1);
//	入力機器環境の解放
	di.reset( );
//	画像処理環境の解放
	dg.reset( );
}
//┌───────────────────────────────────────────┐
//｜同名のタスクを全て消滅させる														  ｜
//｜履歴		：2010/02/27	須賀康之	作成										　｜
//└───────────────────────────────────────────┘
bool GameEngine::KillAll(const tstring& k_)
{
	bool rtv = false;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		if((*it)->Name( ) == k_){
			(*it)->Kill( );
			rtv = true;
		}
	}
	return rtv;
}
//	同名のタスクを全て停止する
bool GameEngine::StopAll(const tstring& k_)
{
	bool rtv = false;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		if((*it)->Name( ) == k_){
			(*it)->Kill( );
			rtv = true;
		}
	}
	return rtv;
}
//	同名のタスクを全て一時無効にする
bool GameEngine::SuspendAll(const tstring& k_)
{
	bool rtv = false;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		if((*it)->Name( ) == k_){
			(*it)->Kill( );
			rtv = true;
		}
	}
	return rtv;
}

//┌───────────────────────────────────────────┐
//｜タスクを登録																		  ｜
//｜履歴		：2010/02/27	須賀康之	作成										　｜
//└───────────────────────────────────────────┘
void GameEngine::PushBack(const BTask::SP& t_)
{
	addtasks.push_back(t_);
}
