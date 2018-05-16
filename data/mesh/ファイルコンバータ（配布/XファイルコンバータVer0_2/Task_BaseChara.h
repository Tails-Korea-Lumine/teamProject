#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//全キャラクタのベースとなるクラス（仮)
//全キャラクタが共通で持つ変数はここで設定する。
//（全キャラクタでなくとも、頻繁に利用される変数があるならここに加える）
//-----------------------------------------------------------------------------
#include "MyPG.h"

class BChara : public BTask
{
public:
	BChara(tstring n_);
	virtual ~BChara( );

	typedef shared_ptr<BChara>	SP;	//シェアポインタ略名
	typedef weak_ptr<BChara>	WP;	//ウィークポインタ略名
//-----------------------------------------------------------------------------
//タスク内変数の宣言（個別）
//-----------------------------------------------------------------------------
	ML::Vec3	pos;		//キャラクタの座標
	float		footAjast;	//足元判定時の高さ調整値
	ML::Vec3	FootPos( ){ ML::Vec3 w = pos; w.y+=footAjast; return w;}

	float		jumpSpeed;	//上昇＆落下速度
	bool		footHit;	//着地している
	float		dirY, dirX;	//キャラクタの向き
	ML::Mat4x4	matW;		//ワールド変換行列
	ML::Mat4x4	matW_inv;	//ワールド変換行列の逆行列
//-----------------------------------------------------------------------------
//追加関数
//-----------------------------------------------------------------------------
	void UpDate_DrawInfo( );	//描画パラメータ更新
	virtual  DG::cMesh::SP  Mesh( ) = 0;	//メッシュを返す
//	レイとメッシュ（Ｂボックス）の接触判定を行う
	bool HitCheck_BBox_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_);
//	レイとメッシュの接触判定を行う
	bool HitCheck_Mesh_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_,  float&  per1_);

};
