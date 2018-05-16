#include "MyMouse.h"

Mouse::Mouse( )
{
	pos.x = 0;
	pos.y = 0;
	l = false;
	r = false;
	pr_l = false;
	pr_r = false;
}

Mouse::~Mouse( )
{
}
void Mouse::Check( )
{
	pr_l = l;
	pr_r = r;
	l = wml;
	r = wmr;
}
Mouse::ButState Mouse::LBStatus( )
{
	if(l == true){
		if(pr_l == true){	return On;}		//��������
		else{				return Click;}	//������
	}else{
		if(pr_l == true){	return Off;}	//������
		else{				return Non;}	//�����Ă��Ȃ�
	}
}
Mouse::ButState Mouse::RBStatus( )
{
	if(r == true){
		if(pr_r == true){	return On;}		//��������
		else{				return Click;}	//������
	}else{
		if(pr_r == true){	return Off;}	//������
		else{				return Non;}	//�����Ă��Ȃ�
	}
}
POINT Mouse::Pos( ){	return pos;	}
void Mouse::LBStatus(bool s){	wml = s;}
void Mouse::RBStatus(bool s){	wmr = s;}
void Mouse::Pos(POINT& p){		pos = p;}
