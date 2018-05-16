#pragma once
#include <windows.h>
#include <functional>
#pragma	comment(lib,"winmm")	//	�}���`���f�B�A�g���@�\���g�p���邽�߂ɕK�v
#include "DI2012.h"
#include "DG2014.h"
#include "myLib.h"
#include <vector>
#include <map>

//�^�X�N��{�N���X
class BTask
{
	static DWORD uKeyManager;	//���j�[�N�L�[�}�l�[�W��
public:
	enum State{	eActive,	//�ʏ�
				eStop,		//��~�i�X�e�b�v���s��~�j
				eKill,		//����
				eSuspend,	//�T�X�y���h�i�ꎞ�����j
				eStateNon = 0xFFFFFFFF};
	DWORD		uKey;			//�^�X�N���Ɋ��蓖�Ă��郆�j�[�N�L�[
	tstring		name;			//�^�X�N�Ɋ��蓖�Ă閼�O�i�����Ɏg�p�j
	State		state;			//�^�X�N�̏��
	float		drawPriority;	//�Q�c�p�v���C�I���e�B
	float		drawPriority3D;	//3D�p�v���C�I���e�B
	bool		alpha;			//3D�p�v���C�I���e�B

	BTask( ){ }
	BTask(tstring n_, State s_=eActive):name(n_), state(s_), drawPriority(0)
	,drawPriority3D(0), alpha(false){ uKey = uKeyManager++; }
public:
	virtual ~BTask( ){ };
	void T_Step( ){		if(state == eActive){ Step( );}	}
	void T_Draw2D( ){	if(state == eActive || state == eStop){ Draw2D( );}	}
	void T_Draw3D( ){	if(state == eActive || state == eStop){ Draw3D( );}	}
	virtual void Step( )=0;
	virtual void Draw2D( )=0;
	virtual void Draw3D( )=0;
	typedef shared_ptr<BTask>	SP;
	typedef weak_ptr<BTask>		WP;
	void Kill( ){		state = eKill;	}							//���ł�����
	void Suspend( ){	if(state != eKill){ state = eSuspend;	} }	//�T�X�y���h�ɂ���
	void Active( ){		if(state != eKill){ state = eActive;	} }	//�A�N�e�B�u�ɖ߂�
	void Stop( ){		if(state != eKill){ state = eStop;		} }	//��~��Ԃɂ���
	bool KillCheck( ){	return (state == eKill); }					//���ŏ�Ԃ��ۂ����m�F
	tstring& Name( ){return name;}
	float DrawPriority( ){return drawPriority;}
};

//	�Q�[���G���W��
class GameEngine
{
public:
	DWORD			screenWidth;		//	��ʃT�C�Y
	DWORD			screenHeight;		//	��ʃT�C�Y
	DWORD			viewScale;			//	�\���{��
	bool			screenMode;			//	�t���X�N���[�����[�h
	char*			windowTitle;		//	�E�C���h�E�^�C�g��
	char*			windowClassName;	//	�E�B���h�E�N���X��
	bool			quitRequest;		//	GameMain�ɏI���v�����鎞true�ɂ���
	DG::cObj::SP	dg;					//	�O���t�B�b�N���C�u����
	DI::cObj::SP	di;					//	���̓��C�u����
	DWORD			bgColor;			//	�w�i�F

private:
	vector<BTask::SP>  tasks;
	vector<BTask::SP>  addtasks;
public:
	GameEngine(
		DWORD		sw_,	//	��ʃT�C�Y
		DWORD		sh_,	//	��ʃT�C�Y
		DWORD		vs_,	//	�\���{��
		bool		sm_,	//	�t���X�N���[�����[�h
		TCHAR*		wtn_,	//	�E�C���h�E�^�C�g��
		TCHAR*		wcn_)	//	�E�B���h�E�N���X��
	:screenWidth(sw_)
	,screenHeight(sh_)
	,viewScale(vs_)
	,screenMode(sm_)
	,windowTitle(wtn_)
	,windowClassName(wcn_)
	,quitRequest(false)
	,bgColor(0x00FFFFFF)
	{ }
	//	�Q�[������
	void B_Step(HWND wnd_);
	//	������
	bool B_Init(HWND wnd_);
	//	���
	virtual ~GameEngine( );
	//	�^�X�N�̓o�^
	void PushBack(const BTask::SP& t_); 
	//	�����̃^�X�N��S�č폜����
	bool KillAll(const tstring& k_);
	//	�����̃^�X�N��S�Ē�~����
	bool StopAll(const tstring& k_);
	//	�����̃^�X�N��S�Ĉꎞ�����ɂ���
	bool SuspendAll(const tstring& k_);

	//������������������������������������������������������������������������������������������
	//�b�����Ώۂ̒�����w��ԍ��ڂ����o													  �b
	//�b����		�F2013/02/27	�{��N�V	�쐬										�@�b
	//������������������������������������������������������������������������������������������
	template <class T>
	shared_ptr<T> GetTask(const tstring& k_, DWORD n_)
	{
		for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
			if((*it)->KillCheck( ) == false){
				if((*it)->Name( ) == k_){
					if(n_ == 0){
						return static_pointer_cast<T>(*it);
					}
					else{--n_;}
				}
			}
		}
		shared_ptr<T> rtv;
		return  rtv;
	}
	virtual bool Init(HWND wnd_)=0;
	virtual void Step( )=0;

	//3DPG1�Ή��ɂ��ǉ�
	//2D�`����̃f�t�H���g�p�����[�^�ݒ�
	virtual void Set2DDrawState( )=0;
	//3D�`����̃f�t�H���g�p�����[�^�ݒ�
	virtual void Set3DDrawState( )=0;

	//������������������������������������������������������������������������������������������
	//�b�w�薼�i�����w��j�̑Ώۂ����o���A���̃x�N�^�[��Ԃ�								  �b
	//�b����		�F2013/03/21	�{��N�V	�쐬										�@�b
	//������������������������������������������������������������������������������������������
	template <class T>
	bool GetTasks(vector<shared_ptr<T>>& tasks_,  const vector<tstring> tgNames_, DWORD exclusionKey_ = 0)
	{
		bool rtv = false;
		
		//���O�̈ꗗ���[�v
		for(auto itN = tgNames_.begin( ); itN != tgNames_.end( ); ++itN){
			//�^�X�N�̈ꗗ���[�v
			for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
				if((*it)->KillCheck( ) == false){
					if((*it)->Name( ) == (*itN)){
						if(exclusionKey_ != (*it)->uKey){
							tasks_.push_back( static_pointer_cast<T>(*it) );
							rtv = true;
						}
					}
				}
			}
		}
		return rtv;
	}

};



