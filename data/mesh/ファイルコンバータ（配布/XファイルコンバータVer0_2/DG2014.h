#pragma warning(disable:4996)
#pragma once
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dx9d.lib")
#pragma comment(lib,"d3d9.lib")
#include <d3d9.h>
#include <d3dx9.h>
#include "myLib.h"
#include <string>

//┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//┃画像制御系機能群																		　┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
namespace DG
{
	//例外として投げられるエラー情報
	enum eErr
	{
		ERR_CREATE,		//生成失敗
		ERR_NOTDEVICE,	//デバイスが取得的ない
		//↑アバウト過ぎるので必要に応じて分化する事
	};
	class cObj
	{
		LPDIRECT3DDEVICE9	d3dDevice;	//	デバイスオブジェクト
		LPDIRECT3D9			d3d;		//	3Dオブジェクト
		D3DCAPS9			caps;		//	グラフィックカードの能力
		LPD3DXSPRITE		sprite;		//	スプライトオブジェクト

		typedef weak_ptr<cObj> WP;
		static cObj::WP		winst;		//	１つしか存在できないインスタンスのウィークポインタ
		cObj( ){ }
		cObj(	HWND	hw_,
				int		w_,
				int		h_,
				bool	sm_);	
	public:
		typedef shared_ptr<cObj> SP;
		~cObj( );
		//	DGライブラリを生成する
		static cObj::SP Create(	HWND		hw_,	//	ウィンドウハンドル
								int			w_,		//	横幅
								int			h_,		//	縦幅
								bool		sm_);	//	スクリーンモード
		static cObj::SP GetInst( );
//		static bool DeleteInst( );
	//	画面描画（開始）
		BOOL  Begin(	DWORD color_ = 0x000000);	//	塗りつぶし色
	//	3D描画の許可を得る
		void  Begin3D( );
	//	3D描画のを終了する
		void  End3D( );
	//	2D描画の許可を得る
		void  Begin2D( );
	//	2D描画のを終了する
		void  End2D( );
	//	画面描画（終了）
		void  End( );
	//	デバイス
		LPDIRECT3DDEVICE9 Device( );
		LPD3DXSPRITE Sprite( );
	};
	class cMesh
	{
		DWORD					numMaterials;	//	マテリアル情報数
		LPD3DXBUFFER			materials;		//	マテリアル情報
		LPDIRECT3DTEXTURE9*		textures;		//	テクスチャ情報
		LPD3DXMESH				mesh;			//	メッシュ情報
		D3DXMATRIX				matDef;			//  デフォルトの変換行列（サイズ・基点位置・向きの違いを吸収）
		ML::Box					bBox;			//	バウンディングボックス

		cMesh( ){ }
		cMesh(	const tstring&		fpath_,
				const D3DXMATRIX&	matDef_);			
	public:
		~cMesh( );
		typedef shared_ptr<cMesh> SP;
		typedef weak_ptr<cMesh> WP;
	//	メッシュを生成する
		static SP Create(	const tstring&			fpath_,
							const D3DXMATRIX&		matDef_ = D3DXMATRIX());
	//	メッシュを表示する（簡易機能の為、長期の使用は推奨しない）
		void Draw(	const ML::Vec3&	pos_,			//	表示座標
					float				rotY_ = 0.0f);	//	回転角度（ラジアン）
	//	メッシュを表示する
		void Draw(	const D3DXMATRIX&	mat_);
	//	レイとメッシュ(Ｂボックス)の接触判定を行う
		ML::Box  BBox( ){ return bBox;}
		bool HitCheck_BBox_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_);
	//	レイとメッシュの接触判定を行う
		bool HitCheck_Mesh_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_,  float&  per1_);

		bool  W_SOB(const tstring&	fpath_);
	};
	class cImage
	{
		LPDIRECT3DTEXTURE9		tex;
		float					rot;
		D3DXVECTOR2				rotC;

		cImage( ){ }
		cImage(		const tstring&	fpath_,
					DWORD			color_);			
	public:
		~cImage( );
		typedef shared_ptr<cImage> SP;
		typedef weak_ptr<cImage> WP;
	//	テクスチャを生成する
		static SP Create(	const tstring&	fpath_,
							DWORD			color_ = 0xFF000000);
	//	テクスチャを表示する
		void Draw(	const RECT&		draw_,	//	描画先
					const RECT&		src_,	//	転送元
					DWORD			color_ = 0xFFFFFFFF);	//不透明度＋色
		void Rotation(	float				r_ = 0.0f,					//	回転量（ラジアン）
						const D3DXVECTOR2&	c_ = D3DXVECTOR2(0, 0));	//	回転軸（draw相対座標）
	};
	class cCamera
	{
		cCamera( ){ }
		cCamera(	const ML::Vec3&	tg_,	//	被写体の位置
					const ML::Vec3&	pos_,	//	カメラの位置
					const ML::Vec3&	up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
					float				fov_,	//	視野角
					float				np_,	//	前クリップ平面（これより前は映らない）
					float				fp_,	//	後クリップ平面（これより後ろは映らない）
					float				asp_);	//	アスペクト比（画面の比率に合わせる　横÷縦）			
	public:
	//	ビュー情報（方向関連）
		ML::Vec3 target;			//	被写体の位置
		ML::Vec3 pos;			//	カメラの位置
		ML::Vec3 up;				//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
	//	射影情報（視野範囲関連）
		float fov;					//	視野角
		float nearPlane;			//	前クリップ平面（これより前は映らない）
		float forePlane;			//	後クリップ平面（これより後ろは映らない）
		float aspect;				//	アスペクト比（画面の比率に合わせる　横÷縦）
	//	行列情報
		D3DXMATRIX  matView, matProj;

		~cCamera( );
		typedef shared_ptr<cCamera> SP;
	//	カメラを生成する
		static SP Create(	const ML::Vec3&	tg_,	//	被写体の位置
							const ML::Vec3&	pos_,	//	カメラの位置
							const ML::Vec3&	up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
							float				fov_,	//	視野角
							float				np_,	//	前クリップ平面（これより前は映らない）
							float				fp_,	//	後クリップ平面（これより後ろは映らない）
							float				asp_);	//	アスペクト比（画面の比率に合わせる　横÷縦）	
	//	カメラの設定
		void UpDate( );
	};

	class cFont
	{
		LPD3DXFONT		font;

		cFont( ){ }
		cFont(	const tstring&	fname_,
				WORD			width_,
				WORD			height_,
				WORD			weight_,
				BYTE			charSet_);			
	public:
		~cFont( );
		typedef shared_ptr<cFont> SP;
		typedef weak_ptr<cFont> WP;
	//	フォントを生成する
		static SP Create(	const tstring&	fname_,
							WORD			width_,
							WORD			height_, 
							WORD			weight_ = 500,
							BYTE			charSet_ = SHIFTJIS_CHARSET);
	//	文字列を表示する
		void Draw(	const RECT&		draw_,	//	描画先
					const tstring&	tex_,
					DWORD			color_ = 0xFFFFFFFF,
					UINT			uFormat_ = DT_LEFT);
		enum FRAME{ x1, x2, x4};
	//	フレーム付き文字列を表示する
		void DrawF(	const RECT&		draw_,	//	描画先
					const tstring&	tex_,
					FRAME			mode_ = x1,
					DWORD			color = 0xFFFFFFFF,
					DWORD			fColor_ = 0xFF000000,
					UINT			uFormat_ = DT_LEFT);
	};
}
