#pragma once
#include "GameEngine_Ver3_7.h"
#include <fstream>
#include <sstream>

//ゲーム内全域で使用する構造体などを定義する
//-----------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------
namespace  MyPG
{
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//----------------------------------------------
	//カメラ基本形
	class Camera
	{
		Camera( ){ }
		Camera(
				const ML::Vec3&		tg_,	//	被写体の位置
				const ML::Vec3&		pos_,	//	カメラの位置
				const ML::Vec3&		up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
				float				fov_,	//	視野角
				float				np_,	//	前クリップ平面（これより前は映らない）
				float				fp_,	//	後クリップ平面（これより後ろは映らない）
				float				asp_);	//	アスペクト比（画面の比率に合わせる　横÷縦）			
	public:
	//	ビュー情報（方向関連）
		ML::Vec3 target;			//	被写体の位置
		ML::Vec3 pos;			//	カメラの位置
		ML::Vec3 up;				//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
	//	射影情報（視野範囲関連）
		float fov;					//	視野角
		float nearPlane;			//	前クリップ平面（これより前は映らない）
		float forePlane;			//	後クリップ平面（これより後ろは映らない）
		float aspect;				//	アスペクト比（画面の比率に合わせる　横÷縦）
	//	行列情報
		ML::Mat4x4  matView, matProj;
		~Camera( );
		typedef shared_ptr<Camera> SP;
	//	カメラを生成する
		static SP Create(
				const ML::Vec3&		tg_,	//	被写体の位置
				const ML::Vec3&		pos_,	//	カメラの位置
				const ML::Vec3&		up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
				float				fov_,	//	視野角
				float				np_,	//	前クリップ平面（これより前は映らない）
				float				fp_,	//	後クリップ平面（これより後ろは映らない）
				float				asp_);	//	アスペクト比（画面の比率に合わせる　横÷縦）	
	//	カメラの設定
		void UpDate( );
	};
	//----------------------------------------------
	class MyGameEngine : public GameEngine
	{
	public:
		MyGameEngine( );
	//ゲームエンジンに追加したものの初期化と開放
		bool Initialize(HWND wnd_);
		~MyGameEngine( );
	//ゲームエンジンに追加したもののステップ処理
		void UpDate( );

	//3DPG1対応により追加
		//2D描画環境のデフォルトパラメータ設定
		void Set2DRenderState(DWORD l_);
		//3D描画環境のデフォルトパラメータ設定
		void Set3DRenderState(DWORD l_);



	//ゲームエンジンに追加したいものは下に加える
	//----------------------------------------------
		MyPG::Camera::SP		camera[4];		//	カメラ
		D3D10_VIEWPORT			vp3D_L0;
		D3D10_VIEWPORT			vp3D_L1;
		D3D10_VIEWPORT			vp3D_L2;
		D3D10_VIEWPORT			vp3D_L3;
		D3D10_VIEWPORT			vp3D_BF;
		D3D10_VIEWPORT			vp3D_AF;
		
		//shared_ptr<BEffect::SP> effmaneger;

	//----------------------------------------------
	};
}
extern MyPG::MyGameEngine* ge;


