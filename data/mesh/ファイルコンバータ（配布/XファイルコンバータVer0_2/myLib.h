#pragma warning(disable:4996)
#pragma once
#include <tchar.h>
#include <windows.h>
#include <string.h>
#include <d3dx9.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <map>
#include <memory>

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;
typedef std::basic_stringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstringstream;
typedef std::basic_ifstream<TCHAR> tifstream;


using namespace std;
using namespace std::tr1;


namespace ML
{
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

	typedef D3DXVECTOR3 Vec3;
	typedef D3DXVECTOR2 Vec2;
	typedef D3DXMATRIX	Mat4x4;
	typedef D3DXQUATERNION QT;
	//クォータニオン生成サポート
	QT  QT_RotAx(ML::Vec3&  v_,  float  r_);
	QT  QT_RotX(float  rX_);
	QT  QT_RotY(float  rY_);
	QT  QT_RotZ(float  rZ_);
//	直方体（実数型）
	class Box
	{
	public:
		Vec3 f;	//	前部
		Vec3 r;	//	後部
		Box( ) : f( ), r( ){};
		//	各XYZ値共に値が f<=rの関係であること（後部の方が値が大きい）
		Box(const Vec3& f_, const Vec3& r_):f(f_),r(r_){};
	//	ボックスの移動を行う（OffsetRectのBox番）
		void Offset(const Vec3& v_){f+=v_; r+=v_;};
	//	２つのボックスの重なりを判定する
		bool HitCheck(const Box& b_)
		{
			if(f.x < b_.r.x && b_.f.x < r.x){
				if(f.y < b_.r.y && b_.f.y < r.y){
					if(f.z < b_.r.z && b_.f.z < r.z){
						return true;
					}
				}
			}
			return false;
		}

	//	オペレータ (+=がオフセットと同等)
		Box operator-(void) const{	Box w(-f, -r);	return w;}

		Box operator+(const Vec3&	v_){	Box w = (*this);	w.Offset(v_);				return w;}
		Box operator-(const Vec3&	v_){	Box w = (*this);	w.Offset(-v_);				return w;}
		Box operator*(float			v_){	Box w = (*this);	w.f *= v_;	w.r *= v_;		return w;}

		Box operator+=(const Vec3&	v_){	(*this) = (*this) + v_;		return (*this);}
		Box operator-=(const Vec3&	v_){	(*this) = (*this) - v_;		return (*this);}
		Box operator*=(float		v_){	(*this) = (*this) * v_;		return (*this);}
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
	//	メソッドにReleaseを持つオブジェクトの解放を行う
	//	Release()メソッド実行時に内部でdeleteしている物のみ有効
	template<class T>
	static void SafeRelease(T** obj_)
	{
		if(*obj_ != NULL)
		{
			(*obj_)->Release();
			(*obj_) = NULL;
		}
	}
	static RECT Rect(int x, int y, int w, int h)
	{
		RECT t = {x, y, x + w, y + h};
		return t;
	}
}


//XNA COLLISION
//#include "xnacollision.h"

namespace ML
{
namespace COL	//COLLISION
{

}//COLLISION
}

