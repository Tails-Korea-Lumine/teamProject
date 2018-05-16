#include "myLib.h"

namespace ML
{
	QT  QT_RotAx(ML::Vec3&  v_,  float  r_){
		ML::QT  an;	return *D3DXQuaternionRotationAxis(&an, &v_, r_);}
	QT  QT_RotX(float  rX_){ return QT_RotAx(ML::Vec3(1,0,0), rX_);}
	QT  QT_RotY(float  rY_){ return QT_RotAx(ML::Vec3(0,1,0), rY_);}
	QT  QT_RotZ(float  rZ_){ return QT_RotAx(ML::Vec3(0,0,1), rZ_);}

namespace COL
{

}//COLLISION
}