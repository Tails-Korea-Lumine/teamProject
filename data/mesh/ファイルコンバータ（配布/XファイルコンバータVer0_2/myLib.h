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
	//�N�H�[�^�j�I�������T�|�[�g
	QT  QT_RotAx(ML::Vec3&  v_,  float  r_);
	QT  QT_RotX(float  rX_);
	QT  QT_RotY(float  rY_);
	QT  QT_RotZ(float  rZ_);
//	�����́i�����^�j
	class Box
	{
	public:
		Vec3 f;	//	�O��
		Vec3 r;	//	�㕔
		Box( ) : f( ), r( ){};
		//	�eXYZ�l���ɒl�� f<=r�̊֌W�ł��邱�Ɓi�㕔�̕����l���傫���j
		Box(const Vec3& f_, const Vec3& r_):f(f_),r(r_){};
	//	�{�b�N�X�̈ړ����s���iOffsetRect��Box�ԁj
		void Offset(const Vec3& v_){f+=v_; r+=v_;};
	//	�Q�̃{�b�N�X�̏d�Ȃ�𔻒肷��
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

	//	�I�y���[�^ (+=���I�t�Z�b�g�Ɠ���)
		Box operator-(void) const{	Box w(-f, -r);	return w;}

		Box operator+(const Vec3&	v_){	Box w = (*this);	w.Offset(v_);				return w;}
		Box operator-(const Vec3&	v_){	Box w = (*this);	w.Offset(-v_);				return w;}
		Box operator*(float			v_){	Box w = (*this);	w.f *= v_;	w.r *= v_;		return w;}

		Box operator+=(const Vec3&	v_){	(*this) = (*this) + v_;		return (*this);}
		Box operator-=(const Vec3&	v_){	(*this) = (*this) - v_;		return (*this);}
		Box operator*=(float		v_){	(*this) = (*this) * v_;		return (*this);}
	};
	//������������������������������������������������������������������������������������������
	//�b�t�@�C���p�X����A�t�@�C�������폜���ăp�X���������o��							�@�b
	//�b���ӁI�p�X�Ƀt�@�C�������܂�ł��Ȃ��ꍇ�̓���͕ۏႵ�Ȃ�							�@�b
	//�b����		�F2010/01/15	�{��N�V	�쐬										�@�b
	//������������������������������������������������������������������������������������������
	static tstring GetPath(const tstring& fp_)
	{
		tstring s = fp_;
		tstring::size_type id;
		id = s.find_last_of(_T("/"));
		if(tstring::npos == id){	id = s.find_last_of(_T("\\"));}
		if(tstring::npos != id){	s = s.substr(0, id);}
		return s;
	}
	//	���\�b�h��Release�����I�u�W�F�N�g�̉�����s��
	//	Release()���\�b�h���s���ɓ�����delete���Ă��镨�̂ݗL��
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

