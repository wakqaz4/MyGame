#ifndef _CANNON_CONFIG_H_
#define _CANNON_CONFIG_H_

#define	MAX_PLAYER_COUNT		5
#define MAX_TURN_COUNT			10
#define PI						3.1415927f

enum GAME_STATE
{
	PREPARING,					//��Ϸ��ʼ֮ǰ�ĵȴ����ȴ�����������Ӻ�
	DOWN_COUNTING,				//������Ҿ���������ʱ����
	RUNNING,					//��Ϸ�����У���ǰ��ҿ��ж�
	WAITING,					//��ǰ����ѷ����ڵ����ȴ��ڵ����
	PAUSED,						//��Ϸ��ͣ��
	FINISHED,					//ĳһ��ʤ��
	GAME_STATE_COUNT
};

static const float STATE_INTERVAL[GAME_STATE_COUNT] =
{
	10.0f,
	3.0f,
	9.0f,
	10.0f,
	20.0f,
	10.0f
};

const float BOMB_LIFE_SPAN = 5;
#endif //_CANNON_CONFIG_H_