#pragma once

//·¢ÅÆ¶ÑÀà
class Cards{
public:
	Cards();
	void RandCards(void);//Ï´ÅÆ
	int GetCard();//Ä¨ÅÆ
	int GetRemain(void){ return remain; };

private:
	int cards[54];//ÅÆÊı×é
	int remain;//Ê£ÓàÅÆÊı
};