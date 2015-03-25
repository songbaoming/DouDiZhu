#pragma once

//玩家类
class Player{
	friend class Game;
	friend class Scene;
public:
	Player(Game &game);

	void NewGame(void);//开始新的一局，做一些初始化集合等的操作
	void ClearAnalyse(void);//清空分析牌集合
	int GetBaseScore(int questioned,int nowscore);//本局是否想当地主，并给出基本分
	void AddCard(int num){ cards.insert(num); }//抹牌
	int GetRemain(void){ return cards.size(); }//剩余牌数
	bool IsValid(void);//判断选择牌是否合格
	void AnalyseSelection(void);//分析选择牌类型及总权值
	void DivideIntoGroups(void); //分析并拆分牌型
	void ThreeplusAndAirplane(void);//从分析后的基本牌型中组合三带一和飞机
	void DeleteUnkown(void);//删除牌型集合中未知类型
	void SelectCards(bool hint=false);//AI选牌
	void Myself();//直接出牌
	void Friend();//跟友方牌
	void Enemy(bool hint);//跟敌方牌
	void NeedSigle();//拆出单张
	void NeedDouble();
	void NeedSigleSeq();
	void NeedThreePlus();
	void NeedAirplane();
	bool Discard(void); //AI出牌
	bool HumanDiscard();//玩家出牌
	bool DiscardAndClear();//出牌并重置相应结构
	//void Hint(void); //提示牌
	void Pass(void);//过牌，重置相应结构
	//给定权值，从集合中查找相应0-53数字，然后从集合中删除并返回该数字；不存在或无效返回-1
	int ValueToNum(std::set<int> &cardscopy, int value);
	void FreshenMap(std::map<int, int> &m);//删除分析堆中数量为零的元素
	static bool MyCompare(CardGroup *c1, CardGroup *c2);//对分析后牌集合排序的回调函数

private:
	Game &game;//游戏对象
	bool test;//是否试过送下家走
	bool nodiscard;//不出标志
	int score;//玩家当前分数
	std::set<int> cards;//手牌
	std::vector<CardGroup*> analyse;//分析后拆分的牌型集合
	CardGroup selection;//选择牌的集合
	CardGroup discard;//打出的牌的集合
};