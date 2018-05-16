#pragma warning(disable:4996)
#pragma once
#pragma warning( disable : 4005 )
#include <windows.h>
#include <d3dx10.h>
#include <d3dCompiler.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma warning( default : 4005 )


#include "DG2014_Texture.h"
#include <string>

namespace DG_
{
	//-----------------------------------------------------------------------
	//2D描画用テクスチャ補助クラス
	class Image
	{
	public:
		static  bool  dt;	//描画を行った際はtrue、
	//-----------------------------------------------------------------------
	public:
		typedef shared_ptr<Image>    SP;
		typedef weak_ptr<Image>      WP;
	//-----------------------------------------------------------------------
	private:
		Image( );
	//-----------------------------------------------------------------------
	private:
		bool  Initialize(const  string&  fpath_);
	//-----------------------------------------------------------------------
	private:
		Texture::SP		tex;
		float			angle;
		ML::Vec2		center;
		float			sw;
		float			sh;
	//-----------------------------------------------------------------------
	public:
		~Image( );
		POINT  Size( );
	//-----------------------------------------------------------------------
	public:
		//生成する
		static SP Create(const string&  fpath_);
		//重複する画像を別途読み込む
		static SP CreateRep(const string&  fpath_);
	//-----------------------------------------------------------------------
	public:
		//回転
		void  Rotation(float  a_, const ML::Vec2&  c_);
	//-----------------------------------------------------------------------
	public:
		//描画
		void  Draw(const ML::Box2D&  d_, const ML::Box2D&  s_, const ML::Color&  c_ = ML::Color(1, 1, 1, 1));
//		void  Draw( );
	//-----------------------------------------------------------------------
	private:
		static unordered_map<string, WP> rep;
	};
}
