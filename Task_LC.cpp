//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#include "Task_LC.h"
#include "MyGameMain.h"

namespace  LC
{
	//-----------------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize(  )
	{
		__super::Initialize(defGroupName,  defName,  true);	//スーパークラス初期化

		//★データ初期化
		MyGameMain_Initalize( );


		//★タスクの生成

		return  true;
	}
	//-----------------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		MyGameMain_Finalize( );

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-----------------------------------------------------------------------------
	//「実行」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		MyGameMain_UpDate( );
	}
	//-----------------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_BF()
	{
		MyGameMain_Render2D_BF();
	}
	void  Object::Render2D_AF()
	{
		MyGameMain_Render2D_AF();
	}
	//-----------------------------------------------------------------------------
	//「３Ｄ描画」１フレーム毎に行う処理
	void  Object::Render3D_L0()
	{
		MyGameMain_Render3D_L0();
	}
	void  Object::Render3D_L1()
	{
		MyGameMain_Render3D_L1();
	}
	void  Object::Render3D_L2()
	{
		MyGameMain_Render3D_L2();
	}
	void  Object::Render3D_L3()
	{
		MyGameMain_Render3D_L3();
	}
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
//基本的に変更不要なメソッド
//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-----------------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  gameEnginePushBack_)
	{
		Object::SP  ob  =  Object::SP(new  Object( ));
		if(ob){
			ob->me  =  ob;
			if (gameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
			}
			if( !ob->B_Initialize( ) ){
				ob->Kill( );//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-----------------------------------------------------------------------------
	bool  Object::B_Initialize( )
	{
		return  this->Initialize( );
	}
	//-----------------------------------------------------------------------------
	Object::~Object( ){		this->B_Finalize( );	}
	bool  Object::B_Finalize( )
	{	
		auto  rtv = this->Finalize( );
		return  rtv;
	}
	//-----------------------------------------------------------------------------
	Object::Object( ){	}
}