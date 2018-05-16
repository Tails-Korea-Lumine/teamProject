#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#include  "MyPG.h"

namespace  LC
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName(	"本編");	//グループ名
	const  string  defName(			"本体");	//タスク名
	//-----------------------------------------------------------------------------
	class  Object  :  public  BTask
	{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object( );
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		static  Object::SP  Create(bool gameEnginePushBack_);	//生成窓口 引数はtrueでタスクシステムへ自動登録
	private:

		Object( );
		bool  B_Initialize( );
		bool  B_Finalize( );
		bool  Initialize( );	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate( );		//「実行」１フレーム毎に行う処理
		void  Render2D_BF();	//「2D描画」１フレーム毎に行う処理
		void  Render2D_AF();	//「2D描画」１フレーム毎に行う処理
		void  Render3D_L0();	//「3D描画」１フレーム毎に行う処理
		void  Render3D_L1();	//「3D描画」１フレーム毎に行う処理
		void  Render3D_L2();	//「3D描画」１フレーム毎に行う処理
		void  Render3D_L3();	//「3D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する
	};
}