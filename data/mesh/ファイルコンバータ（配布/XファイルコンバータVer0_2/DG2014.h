#pragma warning(disable:4996)
#pragma once
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dx9d.lib")
#pragma comment(lib,"d3d9.lib")
#include <d3d9.h>
#include <d3dx9.h>
#include "myLib.h"
#include <string>

//����������������������������������������������������������������������������������������������
//���摜����n�@�\�Q																		�@��
//����������������������������������������������������������������������������������������������
namespace DG
{
	//��O�Ƃ��ē�������G���[���
	enum eErr
	{
		ERR_CREATE,		//�������s
		ERR_NOTDEVICE,	//�f�o�C�X���擾�I�Ȃ�
		//���A�o�E�g�߂���̂ŕK�v�ɉ����ĕ������鎖
	};
	class cObj
	{
		LPDIRECT3DDEVICE9	d3dDevice;	//	�f�o�C�X�I�u�W�F�N�g
		LPDIRECT3D9			d3d;		//	3D�I�u�W�F�N�g
		D3DCAPS9			caps;		//	�O���t�B�b�N�J�[�h�̔\��
		LPD3DXSPRITE		sprite;		//	�X�v���C�g�I�u�W�F�N�g

		typedef weak_ptr<cObj> WP;
		static cObj::WP		winst;		//	�P�������݂ł��Ȃ��C���X�^���X�̃E�B�[�N�|�C���^
		cObj( ){ }
		cObj(	HWND	hw_,
				int		w_,
				int		h_,
				bool	sm_);	
	public:
		typedef shared_ptr<cObj> SP;
		~cObj( );
		//	DG���C�u�����𐶐�����
		static cObj::SP Create(	HWND		hw_,	//	�E�B���h�E�n���h��
								int			w_,		//	����
								int			h_,		//	�c��
								bool		sm_);	//	�X�N���[�����[�h
		static cObj::SP GetInst( );
//		static bool DeleteInst( );
	//	��ʕ`��i�J�n�j
		BOOL  Begin(	DWORD color_ = 0x000000);	//	�h��Ԃ��F
	//	3D�`��̋��𓾂�
		void  Begin3D( );
	//	3D�`��̂��I������
		void  End3D( );
	//	2D�`��̋��𓾂�
		void  Begin2D( );
	//	2D�`��̂��I������
		void  End2D( );
	//	��ʕ`��i�I���j
		void  End( );
	//	�f�o�C�X
		LPDIRECT3DDEVICE9 Device( );
		LPD3DXSPRITE Sprite( );
	};
	class cMesh
	{
		DWORD					numMaterials;	//	�}�e���A�����
		LPD3DXBUFFER			materials;		//	�}�e���A�����
		LPDIRECT3DTEXTURE9*		textures;		//	�e�N�X�`�����
		LPD3DXMESH				mesh;			//	���b�V�����
		D3DXMATRIX				matDef;			//  �f�t�H���g�̕ϊ��s��i�T�C�Y�E��_�ʒu�E�����̈Ⴂ���z���j
		ML::Box					bBox;			//	�o�E���f�B���O�{�b�N�X

		cMesh( ){ }
		cMesh(	const tstring&		fpath_,
				const D3DXMATRIX&	matDef_);			
	public:
		~cMesh( );
		typedef shared_ptr<cMesh> SP;
		typedef weak_ptr<cMesh> WP;
	//	���b�V���𐶐�����
		static SP Create(	const tstring&			fpath_,
							const D3DXMATRIX&		matDef_ = D3DXMATRIX());
	//	���b�V����\������i�ȈՋ@�\�ׁ̈A�����̎g�p�͐������Ȃ��j
		void Draw(	const ML::Vec3&	pos_,			//	�\�����W
					float				rotY_ = 0.0f);	//	��]�p�x�i���W�A���j
	//	���b�V����\������
		void Draw(	const D3DXMATRIX&	mat_);
	//	���C�ƃ��b�V��(�a�{�b�N�X)�̐ڐG������s��
		ML::Box  BBox( ){ return bBox;}
		bool HitCheck_BBox_Ray( const  ML::Vec3&  rp_,   const  ML::Vec3&  rd_);
	//	���C�ƃ��b�V���̐ڐG������s��
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
	//	�e�N�X�`���𐶐�����
		static SP Create(	const tstring&	fpath_,
							DWORD			color_ = 0xFF000000);
	//	�e�N�X�`����\������
		void Draw(	const RECT&		draw_,	//	�`���
					const RECT&		src_,	//	�]����
					DWORD			color_ = 0xFFFFFFFF);	//�s�����x�{�F
		void Rotation(	float				r_ = 0.0f,					//	��]�ʁi���W�A���j
						const D3DXVECTOR2&	c_ = D3DXVECTOR2(0, 0));	//	��]���idraw���΍��W�j
	};
	class cCamera
	{
		cCamera( ){ }
		cCamera(	const ML::Vec3&	tg_,	//	��ʑ̂̈ʒu
					const ML::Vec3&	pos_,	//	�J�����̈ʒu
					const ML::Vec3&	up_,	//	�J�����̏�����������x�N�g���i��̂x�{�Œ�j
					float				fov_,	//	����p
					float				np_,	//	�O�N���b�v���ʁi������O�͉f��Ȃ��j
					float				fp_,	//	��N���b�v���ʁi��������͉f��Ȃ��j
					float				asp_);	//	�A�X�y�N�g��i��ʂ̔䗦�ɍ��킹��@�����c�j			
	public:
	//	�r���[���i�����֘A�j
		ML::Vec3 target;			//	��ʑ̂̈ʒu
		ML::Vec3 pos;			//	�J�����̈ʒu
		ML::Vec3 up;				//	�J�����̏�����������x�N�g���i��̂x�{�Œ�j
	//	�ˉe���i����͈͊֘A�j
		float fov;					//	����p
		float nearPlane;			//	�O�N���b�v���ʁi������O�͉f��Ȃ��j
		float forePlane;			//	��N���b�v���ʁi��������͉f��Ȃ��j
		float aspect;				//	�A�X�y�N�g��i��ʂ̔䗦�ɍ��킹��@�����c�j
	//	�s����
		D3DXMATRIX  matView, matProj;

		~cCamera( );
		typedef shared_ptr<cCamera> SP;
	//	�J�����𐶐�����
		static SP Create(	const ML::Vec3&	tg_,	//	��ʑ̂̈ʒu
							const ML::Vec3&	pos_,	//	�J�����̈ʒu
							const ML::Vec3&	up_,	//	�J�����̏�����������x�N�g���i��̂x�{�Œ�j
							float				fov_,	//	����p
							float				np_,	//	�O�N���b�v���ʁi������O�͉f��Ȃ��j
							float				fp_,	//	��N���b�v���ʁi��������͉f��Ȃ��j
							float				asp_);	//	�A�X�y�N�g��i��ʂ̔䗦�ɍ��킹��@�����c�j	
	//	�J�����̐ݒ�
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
	//	�t�H���g�𐶐�����
		static SP Create(	const tstring&	fname_,
							WORD			width_,
							WORD			height_, 
							WORD			weight_ = 500,
							BYTE			charSet_ = SHIFTJIS_CHARSET);
	//	�������\������
		void Draw(	const RECT&		draw_,	//	�`���
					const tstring&	tex_,
					DWORD			color_ = 0xFFFFFFFF,
					UINT			uFormat_ = DT_LEFT);
		enum FRAME{ x1, x2, x4};
	//	�t���[���t���������\������
		void DrawF(	const RECT&		draw_,	//	�`���
					const tstring&	tex_,
					FRAME			mode_ = x1,
					DWORD			color = 0xFFFFFFFF,
					DWORD			fColor_ = 0xFF000000,
					UINT			uFormat_ = DT_LEFT);
	};
}
