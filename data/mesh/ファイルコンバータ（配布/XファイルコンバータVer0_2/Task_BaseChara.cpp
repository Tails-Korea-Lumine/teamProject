//-----------------------------------------------------------------------------
//全キャラクタのベースとなるクラス（仮)
//-----------------------------------------------------------------------------
#include "Task_BaseChara.h"
//-----------------------------------------------------------------------------
//初期化処理
//機能概要：タスク生成時に１回実行される（素材などの準備を行う）
//-----------------------------------------------------------------------------
BChara::BChara(tstring n_):BTask(n_)
,pos(0,0,0)
,dirY(0), dirX(0)
,jumpSpeed(0)
,footHit(false)
,footAjast(0)			//NEW
{
	D3DXMatrixIdentity(&matW);
	//↑全ての追加変数を0ないし無効の状態に初期化するコードを加える
}
//-----------------------------------------------------------------------------
//解放処理
//機能概要：タスク終了時に１回実行される（素材などの解放を行う）
//-----------------------------------------------------------------------------
BChara::~BChara( )
{
}
//-----------------------------------------------------------------------------
//描画パラメータ更新
//機能概要：描画に関連するパラメータを更新する
//-----------------------------------------------------------------------------
void BChara::UpDate_DrawInfo( )
{
//	ワールド変換行列を更新する
	D3DXMATRIX	matT, matRY, matRX;
//	Y軸に対する回転行列を作成する
	D3DXMatrixRotationY(&matRY, dirY);
	D3DXMatrixRotationX(&matRX, dirX);
//	平行移動行列を作成する
	D3DXMatrixTranslation(&matT, pos.x, pos.y, pos.z);
//	行列の合成
	matW = matRY * matRX * matT;
//	逆行列の生成（ワールド変換の逆行列）
	D3DXMatrixInverse(&matW_inv,  NULL,  &matW);
}
//-----------------------------------------------------------------------------
//レイとメッシュ内のバウンディングボックスの接触判定
bool BChara::HitCheck_BBox_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_)
{
	//レイをメッシュのローカル座標系に変換
	ML::Vec3  rayPos_inv;
	D3DXVec3TransformCoord(&rayPos_inv,  &rp_,  &matW_inv);
	ML::Vec3  rayDir_inv;
	D3DXVec3TransformNormal(&rayDir_inv,  &rd_,  &matW_inv);

	//レイとメッシュのＢボックスとの接触判定
	DG::cMesh::SP  mesh = Mesh( );
	if( mesh &&
		true == mesh->HitCheck_BBox_Ray(rayPos_inv, rayDir_inv)){
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
//	レイとメッシュの接触判定を行う
bool BChara::HitCheck_Mesh_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_,  float&  per1_)
{
	//レイをメッシュのローカル座標系に変換
	ML::Vec3  rayPos_inv;
	D3DXVec3TransformCoord(&rayPos_inv,  &rp_,  &matW_inv);
	ML::Vec3  rayDir_inv;
	D3DXVec3TransformNormal(&rayDir_inv,  &rd_,  &matW_inv);

	//レイとメッシュの接触判定
	DG::cMesh::SP  mesh = Mesh( );
	if( mesh &&
		true == mesh->HitCheck_Mesh_Ray(rayPos_inv, rayDir_inv, per1_)){
		return true;
	}
	return false;
}


