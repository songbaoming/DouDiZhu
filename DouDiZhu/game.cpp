/**************************************************************\
模块：
类Game -> 斗地主.exe
文件：
game.cpp
功能：
游戏类，控制整个游戏的进行。包含游戏进行除界面以外的所有元素
作者：
宋保明
修改历史：
修改人	修改时间	修改内容
-------	-----------	-------------------------------
宋保明	2014.12.5	创建
\**************************************************************/

#include <Windows.h>
#include <fstream>
#include <random>
#include <time.h>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>
#include "winmain.h"
#include "cardgroup.h"
#include "player.h"
#include "cards.h"
#include "scene.h"
#include "game.h"

using namespace std;

Game::Game(HWND hwnd)
: landlord(nullptr)
, curplayer(nullptr)
, lastone(nullptr)
, basescore(0)
, times(0)
, questioned(0)
, hMainWnd(hwnd)
, status(NOTSTART)
, callbegin(0)
{
	for (int i = 0; i < 3; ++i){
		callscore[i] = 0;
		player[i] = new Player(*this);
	}
}

Game::~Game()
{
	for (int i = 0; i < 3; ++i)
		delete player[i];
}
//初始化相关结构
void Game::InitGame()
{
	landlord = curplayer = lastone = nullptr;
	basescore = questioned = 0;
	times = 1;
	for (int i = 0; i < 3; ++i){
		player[i]->NewGame();
	}
	cardheap.RandCards();
	status = GETLANDLORD;
}
//开始新游戏
void Game::GameStart()
{
	InitGame();
	SendCard();
	scene->HideDiscardBtn();
	scene->HideQuestionBtn();
	scene->DrawBackground();
	scene->ShowScene(hMainWnd);;
	status = GETLANDLORD;
	SetTimer(hMainWnd, 1, 500, NULL);

}

void Game::LoadPlayerScore()
{
	ifstream in(szDataFile);

	if (!in)
		return;
	for (int i = 0; i < 3; ++i)
		in >> player[i]->score;
}

void Game::StorePlayerScore()
{
	ofstream out(szDataFile);

	if (!out)
		return;
	for (int i = 0; i < 3; ++i)
		out << player[i]->score << "\n";
}

//获取游戏的当前进度
Status Game::GetStatus()
{
	return status;
}
//获取当前玩家的上家
Player *Game::ProPlayer()
{
	int i;
	for (i = 0; i < 3; ++i){
		if (player[i] == curplayer)
			break;
	}
	return player[(i + 2) % 3];
}
//获取当前玩家的下家
Player *Game::NextPlayer()
{
	return player[NextPlayerNum()];
}
//当前玩家的下家在玩家指针数组中的下标
int Game::NextPlayerNum(void)
{
	int i;
	for (i = 0; i < 3; ++i){
		if (player[i] == curplayer)
			break;
	}
	return (i + 1) % 3;
}

bool Game::IsHumanTurn(void)
{
	return curplayer == player[0];
}

void Game::SendCard(void)
{
	while (cardheap.GetRemain() > 3){
		player[0]->AddCard(cardheap.GetCard());
		player[1]->AddCard(cardheap.GetCard());
		player[2]->AddCard(cardheap.GetCard());
	}
	for (int i = 0; i < 3; ++i)
		landlordcard[i] = cardheap.GetCard();
}
//叫地主
void Game::GetLandlord()
{
	int i = -1;
	if (!questioned){//随机确定开始询问的玩家
		default_random_engine e((UINT)time(nullptr));
		uniform_int_distribution<unsigned> u(0, 2);

		callbegin = i = u(e);
		status = GETLANDLORD;
	}
	else if (questioned == 3){//所有玩家都已询问过
		if (lastone){//给出分数最高的为地主
			curplayer = landlord = lastone;
			lastone = nullptr;
		}
		else{//若均为叫牌，重新开始游戏
			status = NOTSTART;
		}
		scene->ShowScene(hMainWnd);
		if (landlord)//地主已经确定，进入给地主发牌阶段
			status = SENDLANDLORDCARD;
		SetTimer(hMainWnd, 1, 500, NULL);
		return;
	}
	if (i == -1)//不是第一次询问，确定要询问的下家
		i = NextPlayerNum();

	if (i == 0){//询问到真人玩家，需要等待玩家做出选择
		scene->ShowQuestionBtn();
		curplayer = player[0];
		return;
	}
	else{//否则直接调用玩家的AI函数
		curplayer = player[i];
		int result = curplayer->GetBaseScore(questioned, basescore);
		callscore[i] = result;
		if (result == 3){//给出三分就直接当地主
			basescore = result;
			landlord = curplayer;
			lastone = nullptr;
		}
		else if (result > basescore){//否则，给出分数大于上次给分玩家
			basescore = result;
			lastone = curplayer;//就把该玩家记录下来
		}
		++questioned;
		scene->ShowScene(hMainWnd);;
	}
	if (landlord)//地主已确定，进入发地主牌阶段
		status = SENDLANDLORDCARD;
	SetTimer(hMainWnd, 1, 500, NULL);
}
//设置真人玩家叫地主的分数
void Game::SendScore(int result)
{
	callscore[0] = result;
	if (result == 3){
		basescore = result;
		landlord = player[0];
		lastone = nullptr;
	}
	else if (result > basescore){
		basescore = result;
		lastone = player[0];
	}
	++questioned;
	scene->ShowScene(hMainWnd);
	if (landlord)
		status = SENDLANDLORDCARD;
	SetTimer(hMainWnd, 1, 500, NULL);
}
//发地主牌
void Game::SendLandlordCard()
{
	for (auto mem : landlordcard)
		landlord->AddCard(mem);
	scene->DrawBackground();//产生了地主，将地主牌正面显示
	scene->ShowScene(hMainWnd);;
	status = DISCARD;//当前状态为出牌阶段
	SetTimer(hMainWnd, 1, 500, NULL);
}
//出牌
void Game::Discard()
{
	if (lastone == curplayer){//该玩家出牌没人压死，新一轮出牌
		lastone = nullptr;
		for (int i = 0; i < 3; ++i){//清空出牌区
			player[i]->discard.Clear();
			player[i]->nodiscard = false;
		}

	}
	else{//清空当前出牌玩家出牌区
		curplayer->discard.Clear();
		curplayer->nodiscard = false;
	}
	scene->ShowScene(hMainWnd);

	if (curplayer == player[0]){//当前玩家为人
		if (curplayer->selection.count &&
			curplayer->HumanDiscard()){//玩家已选牌并且符合规定
			scene->HideDiscardBtn();
			lastone = curplayer;
			if (curplayer->discard.type == Bomb)//如果出牌为炸弹，增加倍率
				++times;
		}
		else{//否则继续等待玩家选牌
			scene->ShowScene(hMainWnd);
			scene->ShowDiscardBtn();
			return;
		}

	}
	else{//当前出牌方为电脑
		curplayer->SelectCards();
		if (curplayer->Discard())
			lastone = curplayer;
		if (curplayer->discard.type == Bomb)//炸弹
			++times;
	}
	scene->ShowScene(hMainWnd);

	if (lastone->cards.empty())//最后出牌方已无手牌
		status = GAMEOVER;//游戏结束
	else
		curplayer = NextPlayer();//下家继续出牌

	SetTimer(hMainWnd, 1, 500, NULL);
}
//出牌提示
void Game::Hint()
{
	player[0]->selection.Clear();
	player[0]->SelectCards(true);
	if (player[0]->selection.count != 0)
		PostMessage(scene->discand, WM_MYBUTTON, TRUE, 0);
	InvalidateRect(hMainWnd, NULL, FALSE);
}
//过牌
void Game::Pass()
{
	player[0]->Pass();
	curplayer = NextPlayer();//下家出牌

	scene->ShowScene(hMainWnd);
	SetTimer(hMainWnd, 1, 500, NULL);
}

//游戏结束
void Game::GameOver()
{
	int score = basescore * times;
	bool IsPeopleWin = false;

	curplayer = landlord;//把地主设为当前玩家，方便获取上家和下家
	if (landlord->cards.size()){//农民胜利
		landlord->score -= score * 2;
		ProPlayer()->score += score;
		NextPlayer()->score += score;
		if (player[0] != landlord)
			IsPeopleWin = true;
	}
	else{//地主胜利
		landlord->score += score * 2;
		ProPlayer()->score -= score;
		NextPlayer()->score -= score;
		if (player[0] == landlord)
			IsPeopleWin = true;
	}

	scene->ShowScene(hMainWnd);
	if (IsPeopleWin)
		MessageBox(hMainWnd, TEXT("恭喜，您获胜了！"), TEXT("游戏结束"), 0);
	else
		MessageBox(hMainWnd, TEXT("很遗憾，您输了……"), TEXT("游戏结束"), 0);
	GameStart();
}