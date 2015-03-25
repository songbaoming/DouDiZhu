#pragma once

#include <set>
#include "cards.h"

//游戏进度状态
enum Status{
	NOTSTART,//游戏未开始
	GETLANDLORD,//叫地主阶段
	SENDLANDLORDCARD,//发地主牌阶段
	DISCARD,//出牌阶段
	GAMEOVER//游戏结束
};
//游戏主结构
class Game{
	friend class Player;
	friend class Scene;
public:
	Game(HWND hwnd);
	~Game();

	Status GetStatus(void);//获取当前游戏进度状态
	void GameStart(void);//开始新游戏
	void InitGame(void);//初始化相关结构
	void LoadPlayerScore();
	void StorePlayerScore();
	void RegisterScene(Scene *s){ this->scene = s; }
	inline void SendCard(void);//发牌
	void GetLandlord(void);//叫地主
	void SendScore(int result);//设置玩家叫地主分数
	void SendLandlordCard(void);//发地主牌
	Player *ProPlayer(void);//当前玩家的上家
	Player *NextPlayer(void);//当前玩家下家
	int NextPlayerNum(void);//当前玩家的下家在玩家指针数组中的下标
	bool IsHumanTurn(void);//当前玩家为player[0]
	void Discard(void);//出牌
	void Hint(void);//提示
	void Pass(void);//过牌
	void GameOver(void);//游戏结束

private:
	Status status;//游戏进度
	HWND hMainWnd;//主窗口
	Scene *scene;//游戏场景
	Cards cardheap;//发牌堆
	Player *player[3];//真人玩家编号为0
	Player *landlord;//地主
	Player *curplayer;//当前出牌玩家
	Player *lastone;//最后出牌方
	int callscore[3];//各家叫地主的分数
	int callbegin;//第一个叫地主的玩家
	int basescore;//本局基本分
	int times;//本局倍率
	int questioned;//已询问数量
	int landlordcard[3];//本局地主的专属牌存储区
};