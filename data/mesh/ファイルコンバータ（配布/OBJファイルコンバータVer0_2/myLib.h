#pragma warning(disable:4996)
#pragma once
#include <d3d11.h>
#include <d3dx10.h>
#include <d3dx11.h>
#include <d3dCompiler.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dCompiler.lib")

#include <tchar.h>
#include <windows.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <map>
//#include "atlstr.h"

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;
typedef std::basic_stringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstringstream;
typedef std::basic_ifstream<TCHAR> tifstream;
typedef std::basic_ofstream<TCHAR> tofstream;


using namespace std;
using namespace std::tr1;






namespace ML
{
	static const float  PI = 3.14159265358979323846f;
	float ToRadian(float	angle360_);
	typedef D3DXVECTOR4  Vec4;
	typedef D3DXVECTOR3  Vec3;
	typedef D3DXVECTOR2  Vec2;
//	typedef D3DXMATRIX	 Mat4x4;
//	typedef D3DXQUATERNION  QT;

	//クォータニオン
	class QT
	{
	public:
		D3DXQUATERNION q;
		void RotAx(ML::Vec3&  v_,  float  r_){	D3DXQuaternionRotationAxis(&q, &v_, r_);}
		void RotX(float  rX_){					RotAx(ML::Vec3(1,0,0), rX_);}
		void RotY(float  rY_){					RotAx(ML::Vec3(0,1,0), rY_);}
		void RotZ(float  rZ_){					RotAx(ML::Vec3(0,0,1), rZ_);}
		//積		
		QT operator*(QT&  v_){	QT w = (*this);	w.q *= v_.q;	return w;}
		QT operator*=(QT&  v_){	this->q *= v_.q;				return (*this);}
	};
	class Mat4x4
	{
	public:
		D3DXMATRIX  m;
		//単位行列
		void  Identity( ){	D3DXMatrixIdentity(&m);}
		//ビュー行列
		void  LookAtLH(Vec3& pos_,  Vec3&  target_,  Vec3&  up_ ){	D3DXMatrixLookAtLH(	&m, &pos_, &target_, &up_);}
		//射影行列
		void  PerspectiveFovLH(float  fov_,  float  aspect_,  float  nearPlane_,  float  forePlane_ ){	D3DXMatrixPerspectiveFovLH(	&m, fov_, aspect_, nearPlane_, forePlane_);}
		//固定軸回転行列
		void  RotationX(float  r_){ D3DXMatrixRotationX(&m, r_); }
		void  RotationY(float  r_){ D3DXMatrixRotationY(&m, r_); }
		void  RotationZ(float  r_){ D3DXMatrixRotationZ(&m, r_); }
		//任意軸回転行列
		void  RotationAxis(Vec3&  v_, float  r_){ D3DXMatrixRotationAxis(&m, &v_, r_); }
		//クオータニオンで回転行列
		void  RotationQuaternion(QT&  r_){ D3DXMatrixRotationQuaternion (&m, &r_.q); }
		//平行移動行列
		void  Translation(Vec3&  p_){ D3DXMatrixTranslation (&m, p_.x, p_.y, p_.z); }

		//逆行列生成
		Mat4x4 Inverse( ){		Mat4x4  w;	D3DXMatrixInverse(&w.m, NULL, &m);	return  w;}
		//転置行列生成
		Mat4x4 Transpose( ) const{	Mat4x4  w;	D3DXMatrixTranspose(&w.m, &m);	return w;}
		//座標変換
		Vec3  TransformCoord(const Vec3&  p_){ Vec3  w;		D3DXVec3TransformCoord(&w, &p_, &m);	return  w;}
		Vec3  TransformNormal(const Vec3&  v_){ Vec3  w;	D3DXVec3TransformNormal(&w, &v_, &m);	return  w;}
		//積		
		Mat4x4 operator*(Mat4x4&  v_){	Mat4x4 w = (*this);	w.m *= v_.m;	return w;}
		Mat4x4 operator*=(Mat4x4&  v_){	this->m *= v_.m;	return (*this);}
	};


	class Color
	{
	public:
		float  c[4];
		void Set(float  a_,float  r_,float  g_,float  b_){	c[3]=a_; c[2]=b_; c[1]=g_; c[0]=r_;}
		Color(float  a_,float  r_,float  g_,float  b_){		Set(a_, r_, g_, b_);}
		Color(float  c_[4]){								Set(c_[3], c_[2], c_[1], c_[0]);}
		Color( ){											Set(1, 0, 0, 0);}

		const float*  Ptr( ) const{return c;}
	};


	//┌───────────────────────────────────────────┐
	//｜ファイルパスから、ファイル名を削除してパスだけを取り出す							　｜
	//｜注意！パスにファイル名を含んでいない場合の動作は保障しない							　｜
	//｜履歴		：2010/01/15	須賀康之	作成										　｜
	//└───────────────────────────────────────────┘
	static tstring GetPath(const tstring& fp_)
	{
		tstring s = fp_;
		tstring::size_type id;
		id = s.find_last_of(_T("/"));
		if(tstring::npos == id){	id = s.find_last_of(_T("\\"));}
		if(tstring::npos != id){	s = s.substr(0, id);}
		return s;
	}
	//欲しいのはこれだろ？
	static tstring CreateFilePath(const tstring&  base_, const tstring&  fn_)
	{
		tstring s = GetPath(base_);
		s += _T("/");
		s += fn_;
		return s;
	}
	//char文字列をtstringに変換
	static tstring ToTstring(const char  str[])
	{
#ifdef _UNICODE
//		CAtlStringA	strMBCS_FN = str;
//		CAtlStringW	strUnicode_FN = strMBCS_FN;
//		return tstring(strUnicode_FN);
//atlstrが組み込めない残念な環境用
		int  len = ::MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
		WCHAR*  w = new WCHAR[len + 1];
		int  rtv = ::MultiByteToWideChar(CP_ACP, 0, str, -1, w, len);
		if(rtv == 0){return tstring(_T(""));}
		tstring  ws = w;
		delete[] w;
		return ws;
#else
		return tstring(str);
#endif
	}
	//tstringをstringに変換
	static string ToString(const tstring&  str)
	{
#ifdef _UNICODE
		//CAtlStringW	strUnicode_FN = str.c_str( );
		//CAtlStringA	strMBCS_FN = strUnicode_FN;
		//return string(strMBCS_FN);
//atlstrが組み込めない残念な環境用
		int  len = ::WideCharToMultiByte(CP_ACP, 0, str.c_str( ), -1, nullptr, 0, nullptr, nullptr);
		char*  w = new char[len + 1];
		int  rtv = ::WideCharToMultiByte(CP_ACP, 0, str.c_str( ), -1, w, len, nullptr, nullptr);
		if(rtv == 0){return string("");}
		string  ws = w;
		delete[] w;
		return ws;
#else
		return string(str);
#endif
	}


	//	メソッドにReleaseを持つオブジェクトの解放を行う
	//	Release()メソッド実行時に内部でdeleteしている物のみ有効
	template<class T>
	static void SafeRelease(T** obj_)
	{
		if(*obj_ != nullptr)
		{
			(*obj_)->Release();
			(*obj_) = nullptr;
		}
	}
	template<class T>
	static void SafeDelete(T** obj_)
	{
		if(*obj_ != nullptr)
		{
			delete (*obj_);
			(*obj_) = nullptr;
		}
	}
	template<class T>
	static void SafeDeleteArray(T** obj_)
	{
		if(*obj_ != nullptr)
		{
			delete[] (*obj_);
			(*obj_) = nullptr;
		}
	}
	static RECT Rect(int x, int y, int w, int h)
	{
		RECT t = {x, y, x + w, y + h};
		return t;
	}


	int	CheckStrings(TCHAR* s_[], int n_, const tstring& c_);
	void  MsgBox(const tstring&  ms);

}