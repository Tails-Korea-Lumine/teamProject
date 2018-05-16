#include "DG2014_Ver3_51.h"
#include <map>

namespace  DG
{
	DG_::DGObject::SP		obj;
	map<string, DG_::Image::SP>	images;
	bool images_NotObject = false;
	map<string, DG_::Font::SP>	fonts;
	bool fonts_NotObject = false;
	map<string, DG_::Mesh::SP>	meshs;
	bool meshs_NotObject = false;
	map<string, DG_::Texture::SP>	textures;
	bool textures_NotObject = false;

	void  Line_Initialize();
	void  Line_Finalize();

	D3D10_PRIMITIVE_TOPOLOGY	line_priTopo;		//プリミティブトポロジー
	DG_::InputLayout::SP		line_inputLayout;	//頂点情報の入力レイアウト
	string						line_tecName;		//テクニック名
	DG_::VertexBuffer::SP		line_vPos;


	DG_::DGObject&  Object(){return *obj;}
	ID3D10Device&	Device(){	return obj->Device();	}
	ID3D10EffectTechnique&	EffectTec(){ return obj->ETechnique(); }

	//-----------------------------------------------------------------------
	bool  Initialize(
		HWND		hw_,	//	ウィンドウハンドル
		int			w_,		//	横幅
		int			h_,		//	縦幅
		DWORD		ms_,	//	マルチサンプル
		bool		sm_,	//	スクリーンモード
		int			w2D_,	//	2D描画解像度（無効
		int			h2D_)	//	2D描画解像度（無効	
	{
		if (!obj){
			obj = DG_::DGObject::Create(hw_, w_, h_, ms_, sm_, w2D_, h2D_);
			//
			if (obj) {
				Line_Initialize();
			}
		}
		return  obj != nullptr;
	}
	//-----------------------------------------------------------------------
	void  Finalize()
	{
		Line_Finalize();
		Image_Clear();
		Font_Clear();
		Mesh_Clear();
		Texture_Clear();
		obj.reset();
	}
	//-----------------------------------------------------------------------
	//	画面描画（開始）
	BOOL  Begin(ML::Color color_)
	{
		return obj->Begin(color_);
	}
	//-----------------------------------------------------------------------
	//	3D描画の許可を得る
	void  Begin_3D()
	{
		obj->Begin3D();
	}
	//-----------------------------------------------------------------------
	//	3D描画のを終了する
	void  End_3D()
	{
		obj->End3D();
	}
	//-----------------------------------------------------------------------
	//	2D描画の許可を得る
	void  Begin_2D()
	{
		obj->Begin2D();
	}
	//-----------------------------------------------------------------------
	//	2D描画のを終了する
	void  End_2D()
	{
		obj->End2D();
	}
	//-----------------------------------------------------------------------
	//	画面描画（終了）
	void  End()
	{
		obj->End();
	}
	//-----------------------------------------------------------------------
	DG_::EffectState&	EffectState(){ return  obj->EState( ); }
	ID3DX10Sprite&			Sprite(){ return  obj->Sprite( ); }

//---------------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void  Image_Create(
		const string name_,
		const string fileName_)
	{
		auto  w = DG_::Image::Create(fileName_);
		images[name_] = w;
	}
	//-----------------------------------------------------------------------
	void  Image_Draw(
		const string name_,
		ML::Box2D  draw_,
		ML::Box2D  src_,
		const ML::Color&  color_)
	{
		auto it = images.find(name_);
		if (it == images.end()) {
			if (images_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("存在しないオフスクリーンに制御を要求しました"), MB_OK);
			}
			images_NotObject = true;
			return;
		}
		it->second->Draw(draw_, src_, color_);
	}
	//-----------------------------------------------------------------------
	void  Image_Rotation(
		const string name_,
		float  r_,	//	回転量（ラジアン）
		const  ML::Vec2&  c_)	//	回転軸（draw相対座標）
	{
		auto it = images.find(name_);
		if (it == images.end()) {
			if (images_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("存在しないオフスクリーンに制御を要求しました"), MB_OK);
			}
			images_NotObject = true;
			return;
		}
		it->second->Rotation(r_, c_);
	}
	//-----------------------------------------------------------------------
	void  Image_Clear()
	{
		images.clear();
	}
	//-----------------------------------------------------------------------
	void  Image_Erase(const string name_)
	{
		images.erase(name_);
	}
	//-----------------------------------------------------------------------
	POINT Image_Size(const string name_)
	{
		return images[name_]->Size( );
	}

//---------------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void  Font_Create(
		const string	name_,
		const string	fontName_,
		WORD			width_,
		WORD			height_,
		WORD			weight_,
		BYTE			charSet_)
	{
		auto  w = DG_::Font::Create(fontName_, width_, height_, weight_, charSet_);
		fonts[name_] = w;
	}
	//-----------------------------------------------------------------------
	void  Font_Draw(const string name_,
		const ML::Box2D&	draw_,	//	描画先
		const string&		tex_,
		ML::Color			color_,
		UINT				uFormat_)
	{
		auto it = fonts.find(name_);
		if (it == fonts.end()) {
			if (fonts_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("存在しないフォントに制御を要求しました"), MB_OK);
			}
			fonts_NotObject = true;
			return;
		}
		it->second->Draw(draw_, tex_, color_, uFormat_);
	}
	//-----------------------------------------------------------------------
	void  Font_DrawF(
		const string			name_,
		const ML::Box2D&		draw_,	//	描画先
		const string&			tex_,
		FRAME					mode_,
		ML::Color				color_,
		ML::Color				fcolor_,
		UINT					uFormat_)
	{
		auto it = fonts.find(name_);
		if (it == fonts.end()) {
			if (fonts_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("存在しないフォントに制御を要求しました"), MB_OK);
			}
			fonts_NotObject = true;
			return;
		}
		it->second->DrawF(draw_, tex_, (DG_::Font::FRAME)mode_, color_, fcolor_, uFormat_);
	}
	//-----------------------------------------------------------------------
	//現在生成してあるフォントをすべて破棄
	void  Font_Clear()
	{
		fonts.clear();
	}
	//-----------------------------------------------------------------------
	//指定したフォントを破棄
	void  Font_Erase(const string name_)
	{
		fonts.erase(name_);
	}
//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void  Mesh_CreateFromSOBFile(
		const string	name_,
		const  string&  fpath_)
	{
		auto  w = DG_::Mesh::Create_FromSOBFile(fpath_);
		meshs[name_] = w;
	}
	//-----------------------------------------------------------------------
	void Mesh_Draw(
		const string	name_)
	{
		auto it = meshs.find(name_);
		if (it == meshs.end()) {
			if (meshs_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("存在しないメッシュに制御を要求しました"), MB_OK);
			}
			meshs_NotObject = true;
			return;
		}
		it->second->Draw();
	}
	//-----------------------------------------------------------------------
	bool  Mesh_HitRay(
		const string	name_,
		const  ML::Vec3&  pos_,
		const  ML::Vec3&  dir_,
		float&  dist_,
		bool  qmode_)
	{
		auto it = meshs.find(name_);
		if (it == meshs.end()) {
			if (meshs_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("存在しないメッシュに制御を要求しました"), MB_OK);
			}
			meshs_NotObject = true;
			return false;
		}
		return it->second->Hit_Ray(pos_, dir_, dist_, qmode_);
	}
	//-----------------------------------------------------------------------
	bool  Mesh_HitRayBB(
		const string	name_,
		const  ML::Vec3&  pos_,
		const  ML::Vec3&  dir_,
		float&  dist_)
	{
		auto it = meshs.find(name_);
		if (it == meshs.end()) {
			if (meshs_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("存在しないメッシュに制御を要求しました"), MB_OK);
			}
			meshs_NotObject = true;
			return false;
		}
		return it->second->Hit_RayBB(pos_, dir_, dist_);
	}
	//-----------------------------------------------------------------------
	//マテリアル情報を強制的に書き換える（使用は非推奨）
	void  Mesh_UpDateMaterial(
		const string name_,
		const ML::Color&  dif_,
		const ML::Color&  spe_,
		const ML::Color&  amb_,
		const ML::Color&  emi_)
	{
		auto it = meshs.find(name_);
		if (it == meshs.end()) {
			if (meshs_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("存在しないメッシュに制御を要求しました"), MB_OK);
			}
			meshs_NotObject = true;
			return;
		}
		it->second->UpDateMaterial(dif_, spe_, amb_, emi_);
	}

	//-----------------------------------------------------------------------
	void  Mesh_Clear()
	{
		meshs.clear();
	}
	//-----------------------------------------------------------------------
	void  Mesh_Erase(const string name_)
	{
		meshs.erase(name_);
	}
//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void  Line_Draw(
		const ML::Vec3&	posS_,
		const ML::Vec3&	posE_)
	{
		//バーテックスバッファの生成
		ML::Vec3	pos[] = { posS_, posE_ };
		auto  vPos = DG_::VertexBuffer::Create((BYTE*)pos, sizeof(ML::Vec3), _countof(pos));

		//描画パラメータを定数バッファに反映させる
		obj->EState().Update_Params();

		//テクニック選択（呼び出されるシェーダを決定する）
		obj->ETechnique().GetPassByName(line_tecName.c_str( ))->Apply(0);
		//頂点インプットレイアウトをセット
		line_inputLayout->IASet();
		//プリミティブ・トポロジーをセット
		obj->Device().IASetPrimitiveTopology(line_priTopo);

		//頂点バッファを各スロットに接続（素材投入口選択）
		UINT  ofsPos = 0 * sizeof(ML::Vec3);
		obj->Device().IASetVertexBuffers(0, 1, &vPos->buf, &vPos->stride, &ofsPos);
		//レンダリング開始
		obj->Device().Draw(vPos->num, 0);
	}
	void  Line_Initialize()
	{
		//バーテックスシェーダ生成＆頂点インプットレイアウト生成
		//頂点インプットレイアウトを定義	
		D3D10_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};
		//頂点インプットレイアウトの生成
		line_tecName = "Line";
		line_inputLayout = DG_::InputLayout::Create(line_tecName, layout, _countof(layout));
		//プリミティブ・トポロジーを選択
		line_priTopo = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
	}
	void  Line_Finalize()
	{
	}
//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void  Texture_Create(
		const string	name_,
		const  string&  fpath_)
	{
		auto  w = DG_::Texture::Create(fpath_);
		textures[name_] = w;
	}
	void  Texture_PSSet(const string	name_, UINT  slotNum)
	{
		auto it = textures.find(name_);
		if (it == textures.end()) {
			if (textures_NotObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("存在しないテクスチャに制御を要求しました"), MB_OK);
			}
			textures_NotObject = true;
			return;
		}
		it->second->PSSet(slotNum);
	}
	void  Texture_Clear()
	{
		textures.clear();
	}
	void  Texture_Erase(const string name_)
	{
		textures.erase(name_);
	}
}