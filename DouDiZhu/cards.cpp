/**************************************************************\
模块：
	类Cards -> 斗地主.exe
文件：
	cards.cpp
功能：
	游戏进行需要的扑克类，用来表示一副扑克。包含洗牌及发牌函数。
作者：
	宋保明
修改历史：
	修改人	修改时间	修改内容
	-------	-----------	-------------------------------
	宋保明	2014.12.5	创建
\**************************************************************/

#include <stdexcept>
#include <random>
#include <time.h>
#include "cards.h"

using namespace std;

Cards::Cards()
{
	for (int i = 0; i < 54; ++i)
		cards[i] = i;//初始化为0-53
	RandCards();//洗牌一次并重置牌堆剩余牌数
	
	return;
}
//洗牌
void Cards::RandCards(void)
{
	default_random_engine e(time(nullptr));//随机数引擎
	uniform_int_distribution<unsigned> u(0, 53);//整型分布

	for (int i = 0; i < 54; i++){
		int rand = u(e);
		swap(cards[i], cards[rand]);
	}
	remain = 54;
	
	return;
}
//抹牌
int Cards::GetCard(void)
{
	if (!remain)
		throw runtime_error("No more card in cardset!");

	return cards[--remain];
}