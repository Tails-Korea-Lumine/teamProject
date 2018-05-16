#include "myLib.h"

namespace ML
{
	float ToRadian(float  angle360_)
	{
		return  angle360_ * PI / 180.0f;
	}

	int	CheckStrings(TCHAR* s_[], int n_, const tstring& c_)
	{
		int c;
		for(c=0; c < n_; ++c){
			if(c_ == s_[c]){return c;}
		}
		return -1;
	}
	void  MsgBox(const tstring&  ms){
		MessageBox(nullptr, ms.c_str( ), nullptr, MB_OK);
	}
}