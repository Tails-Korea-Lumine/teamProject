#include "GameEngine_Ver1_1.h"
#include "MyPG.h"
#include <time.h>
DWORD BTask::uKeyManager = 10000;
//������������������������������������������������������������������������������������������
//�b�Q�[�����̏�����																	�@�b
//�b����		�F2013/02/25	�{��N�V	�쐬										�@�b
//������������������������������������������������������������������������������������������
bool GameEngine::B_Init(HWND wnd_)
{
//	�^�C�}�[����\��1�~���b�ɐݒ�
	timeBeginPeriod(1);
//	�摜�������̏�����
	dg = DG::cObj::Create(	wnd_,				
							screenWidth,
							screenHeight,
							screenMode);
	if(dg == 0){
	//	���̏������Ɏ��s
		DestroyWindow(wnd_);
		return false;
	}
//	���͋@����̏�����
	di = DI::cObj::Create(	wnd_);
	if(di == 0){
	//	���̏������Ɏ��s
		DestroyWindow(wnd_);
		return false;
	}

	Init(wnd_);
	return true;
}
//������������������������������������������������������������������������������������������
//�b�Q�[������																			�@�b
//�b����		�F2013/02/25	�{��N�V	�쐬										�@�b
//������������������������������������������������������������������������������������������
class cKillCheck{
public:
	// �X�e�[�g��eKill�̎�true��Ԃ�
	bool operator()(BTask::SP t_) const { return t_->KillCheck( ); }
};
void GameEngine::B_Step(HWND wnd_)
{
//	MyPG_Execute( );//�X�P���g���Q�ڍs�ɂ��폜
//	ESC�L�[�������ꂽ	�I���v���t���O���n�m
	if(	GetAsyncKeyState(VK_ESCAPE)){	quitRequest = true;}

//	�^�X�N�̃X�e�b�v����
	Step( );
	for_each(tasks.begin( ), tasks.end( ), mem_fn(&BTask::T_Step));
//	�ǉ����ꂽ�^�X�N�̓o�^
	for(auto it = addtasks.begin( ); it != addtasks.end( ); ++it){
		tasks.push_back((*it));
	}
	addtasks.erase(addtasks.begin( ), addtasks.end( ));

	Set3DDrawState( );
	dg->Begin( bgColor );
//	�^�X�N�̕`�揈��(3D)
	typedef std::pair<float, BTask::SP> dmap;
	multimap<float, BTask::SP> draw3DMM;
	multimap<float, BTask::SP> draw3DAPMM;
	float c=0;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		if(!(*it)->alpha){
			//���߂Ȃ��F��O���牜�ɕ��ׂ�
			draw3DMM.insert( dmap( (*it)->drawPriority3D, (*it) ) );
		}else{
			//���߂���F�������O�ɕ��ׂ�
			draw3DAPMM.insert( dmap( -(*it)->drawPriority3D, (*it) ) );
		}
		c+=1.0f;
	}
	dg->Begin3D( );
	//���߂Ȃ��I�u�W�F�N�g�̕`��
	for(auto it = draw3DMM.begin( ); it != draw3DMM.end( ); ++it){		(*it).second->T_Draw3D( );	}
	//���߂���I�u�W�F�N�g�̕`��
	for(auto it = draw3DAPMM.begin( ); it != draw3DAPMM.end( ); ++it){	(*it).second->T_Draw3D( );	}
	dg->End3D( );


//	�^�X�N�̕`�揈��(2D)
	typedef std::pair<float, BTask::SP> dmap;
	multimap<float, BTask::SP> draw2DMM;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		draw2DMM.insert( dmap( -(*it)->drawPriority, (*it) ) );
	}
	dg->Begin2D( );
	Set2DDrawState( );
	//for_each(draw2DMM.begin( ), draw2DMM.end( ), mem_fn(&BTask::Draw2D));
	for(auto it = draw2DMM.begin( ); it != draw2DMM.end( ); ++it){
		(*it).second->T_Draw2D( );
	}
	dg->End2D( );
	dg->End( );

//	���ŏ�Ԃ̃^�X�N����菜��
	auto endIt = remove_if(tasks.begin( ), tasks.end( ), cKillCheck( ) );	//Kill��Ԃ����X�g�����
	tasks.erase(endIt, tasks.end( ));	//Kill��Ԃ̑S�Ă��폜����

//	�^�X�N�����݂��Ȃ��ꍇ�I���Ƃ���
//	�I���v�������鎞�@�E�B���h�E�̔j�����s��
	if(	!tasks.size( ) ){			DestroyWindow(wnd_);	}
	else if(quitRequest == true){
		tasks.erase(tasks.begin( ), tasks.end( ));	//�c������S�Ẵ^�X�N���폜����
		DestroyWindow(wnd_);
	}
}
//������������������������������������������������������������������������������������������
//�b�Q�[�����̉��																	�@�b
//�b����		�F2010/02/26	�{��N�V	�쐬										�@�b
//������������������������������������������������������������������������������������������
GameEngine::~GameEngine( )
{
//	�^�C�}�[����\�̐ݒ�i1�~���b�P�ʁj����������
	timeEndPeriod(1);
//	���͋@����̉��
	di.reset( );
//	�摜�������̉��
	dg.reset( );
}
//������������������������������������������������������������������������������������������
//�b�����̃^�X�N��S�ď��ł�����														  �b
//�b����		�F2010/02/27	�{��N�V	�쐬										�@�b
//������������������������������������������������������������������������������������������
bool GameEngine::KillAll(const tstring& k_)
{
	bool rtv = false;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		if((*it)->Name( ) == k_){
			(*it)->Kill( );
			rtv = true;
		}
	}
	return rtv;
}
//	�����̃^�X�N��S�Ē�~����
bool GameEngine::StopAll(const tstring& k_)
{
	bool rtv = false;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		if((*it)->Name( ) == k_){
			(*it)->Kill( );
			rtv = true;
		}
	}
	return rtv;
}
//	�����̃^�X�N��S�Ĉꎞ�����ɂ���
bool GameEngine::SuspendAll(const tstring& k_)
{
	bool rtv = false;
	for(auto it = tasks.begin( ); it != tasks.end( ); ++it){
		if((*it)->Name( ) == k_){
			(*it)->Kill( );
			rtv = true;
		}
	}
	return rtv;
}

//������������������������������������������������������������������������������������������
//�b�^�X�N��o�^																		  �b
//�b����		�F2010/02/27	�{��N�V	�쐬										�@�b
//������������������������������������������������������������������������������������������
void GameEngine::PushBack(const BTask::SP& t_)
{
	addtasks.push_back(t_);
}
