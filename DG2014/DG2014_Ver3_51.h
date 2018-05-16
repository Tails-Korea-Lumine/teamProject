#pragma warning(disable:4996)
#pragma once

#include "DG2014_DGObject.h"

#include "DG2014_IndexBuffer.h"
#include "DG2014_InputLayout.h"
#include "DG2014_VertexBuffer.h"
#include "DG2014_VertexContainer.h"

#include "DG2014_Texture.h"
#include "DG2014_Mesh.h"
#include "DG2014_Image.h"
#include "DG2014_Font.h"
#include "DG2014_Effect.h"

namespace
{
	enum{	spriteBufferMax = 4096};
}


namespace DG
{
	//初期化
	bool  Initialize(
		HWND		hw_,	//	ウィンドウハンドル
		int			w_,		//	横幅
		int			h_,		//	縦幅
		DWORD		ms_,	//	マルチサンプル
		bool		sm_,	//	スクリーンモード
		int			w2D_,	//	2D描画解像度（無効
		int			h2D_);	//	2D描画解像度（無効	
	//解放
	void  Finalize();

	//	画面描画（開始）
	BOOL  Begin(ML::Color color_);
	//	3D描画の許可を得る
	void  Begin_3D();
	//	3D描画のを終了する
	void  End_3D();
	//	2D描画の許可を得る
	void  Begin_2D();
	//	2D描画のを終了する
	void  End_2D();
	//	画面描画（終了）
	void  End();

	//エフェクト設定
	DG_::EffectState&	EffectState();
	//スプライト
	ID3DX10Sprite&			Sprite();
	//DGオブジェクト
	DG_::DGObject&		Object();
	//デバイス
	ID3D10Device&	Device();
	//エフェクトテクニック
	ID3D10EffectTechnique&	EffectTec();


//---------------------------------------------------------------------------
	//読み込み＆入れ替え
	void  Image_Create(
		const string name_,
		const string fileName_);
	//表示
	void  Image_Draw(
		const string name_,
		ML::Box2D  draw_,
		ML::Box2D  src_,
		const ML::Color&  c_ = ML::Color(1, 1, 1, 1));
	//回転（設定）
	void  Image_Rotation(
		const string name_,
		float  r_,	//	回転量（ラジアン）
		const  ML::Vec2&  c_);	//	回転軸（draw相対座標）
	//現在生成してある画像をすべて破棄
	void  Image_Clear();
	//指定した画像を破棄
	void  Image_Erase(const string name_);
	//サイズを取得する
	POINT Image_Size(const string name_);
//-----------------------------------------------------------------------
	//フォントを生成する
	void  Font_Create(
		const string	name_,
		const string	fontName_,
		WORD			width_,
		WORD			height_,
		WORD			weight_ = 500,
		BYTE			charSet_ = SHIFTJIS_CHARSET);
	//テキストを表示する
	void  Font_Draw(const string name_,
		const ML::Box2D&	draw_,	//	描画先
		const string&		tex_,
		ML::Color				color_ = ML::Color(1, 1, 1, 1),
		UINT					uFormat_ = DT_LEFT);
	//フレーム付きテキストを表示する
	enum FRAME{ x1, x2, x4 };
	void  Font_DrawF(
		const string			name_,
		const ML::Box2D&		draw_,	//	描画先
		const string&			tex_,
		FRAME					mode_,
		ML::Color				color_ = ML::Color(1, 1, 1, 1),
		ML::Color				fColor_ = ML::Color(1, 0, 0, 0),
		UINT					uFormat_ = DT_LEFT);
	//現在生成してあるフォントをすべて破棄
	void  Font_Clear();
	//指定したフォントを破棄
	void  Font_Erase(const string name_);
//-----------------------------------------------------------------------
	//メッシュを生成する（SOBファイルを読み込んで
	void  Mesh_CreateFromSOBFile(
		const string	name_,
		const  string&  fpath_);
	//メッシュを描画する
	void Mesh_Draw(const string	name_);
	//メッシュとレイの接触判定を行う
	bool  Mesh_HitRay(
		const string	name_,
		const  ML::Vec3&  pos_,
		const  ML::Vec3&  dir_,
		float&  dist_,
		bool  qmode_ = false);
	//メッシュとレイを包むバウンディングボックスの接触判定を行う
	bool  Mesh_HitRayBB(
		const string	name_,
		const  ML::Vec3&  pos_,
		const  ML::Vec3&  dir_,
		float&  dist_);
	//現在生成してあるメッシュをすべて破棄
	void  Mesh_Clear();
	//指定したメッシュを破棄
	void  Mesh_Erase(const string name_);

	//マテリアル情報を強制的に書き換える（使用は非推奨）
	void  Mesh_UpDateMaterial(
		const string name_, 
		const ML::Color&  dif_,
		const ML::Color&  spe_,
		const ML::Color&  amb_,
		const ML::Color&  emi_);

//-----------------------------------------------------------------------
	void  Line_Draw(
		const ML::Vec3&	posS_,
		const ML::Vec3&	posE_);
//-----------------------------------------------------------------------
	//テクスチャを生成する
	void  Texture_Create(
		const string	name_,
		const  string&  fpath_);
	//ピクセルシェーダに登録する
	void  Texture_PSSet(const string	name_, UINT  slotNum);
	//現在生成してあるテクスチャをすべて破棄
	void  Texture_Clear();
	//指定したテクスチャを破棄
	void  Texture_Erase(const string name_);
}