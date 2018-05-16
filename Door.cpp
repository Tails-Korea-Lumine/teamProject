#include "Door.h"

Door::Door()
{
	this->pos = ML::Vec3(0, 0, 0);
	this->hitBase = ML::Box3D(0, 0, 0, 0, 0, 0);
	this->openFlag = false;
	this->cunnected_Breaker.clear();
	this->open_Angle = LR::clear;
}

Door::Door(ML::Vec3 pos, std::vector<Breaker*> b, LR a)
{
	this->pos = pos;
	this->hitBase = ML::Box3D(-25, -100, -25, 50, 100, 50);
	this->openFlag = false;
	this->cunnected_Breaker = b;
	this->open_Angle = a;
}

void Door::Door_Open()
{
	//自分とつながっている全てのブレーカーを検索
	for (auto b : this->cunnected_Breaker)
	{
		//状態を確認する
		if (!b->Get_Now_State())
		{
			return;
		}		
	}
	//全部働いているならドアを開ける
	this->openFlag = true;
}

//void Door::Door_Open(Breaker* b0, Breaker* b1, Breaker* b2)
//{
//	//引数でもらうブレーカーの状態を確認する
//	if (b0->Get_Now_State() && b1->Get_Now_State() && b2->Get_Now_State())
//	{
//		//全部働いているならドアを開ける
//		this->openFlag = true;
//	}
//}

bool Door::Player_Hit_the_Door(const ML::Box3D& hit)
{
	return hit.Hit(this->hitBase.OffsetCopy(this->pos));
}

bool Door::Get_State()
{
	return this->openFlag;
}