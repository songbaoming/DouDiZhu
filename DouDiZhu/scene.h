#pragma once 


class Game;

class Scene{
public:
	Scene(Game *g);
	~Scene();

	SIZE GetSize(void);
	void InitScene(HWND hwnd);
	void ShowScene(HDC hdc);
	void ShowScene(HWND hwnd);
	void ShowQuestionBtn(void);
	void HideQuestionBtn(void);
	void ShowDiscardBtn(void);
	void HideDiscardBtn(void);
	void DrawBackground(void);
	void DrawComputerCards(void);
	void DrawHumanCards(HDC hdc, int highlight = -1);
	void DrawHumanCards(HWND hwnd, int highlight = -1);
	void DrawResult(void);
	void DrawChars(void);
	void MouseMove(POINT point);
	void SelectCard(POINT point);
	void DeleteCard(POINT point);
	int PointInWhich(POINT point);

private:
	Game *game;
	HDC hdcScene;
	HDC hdcBkg;
	HDC hdcHumanCards;//画玩家手牌
	HDC hdcCards;//54张牌的正面
	HDC hdcCardBack;//牌背面
	HDC hdcNoDiscard;//“不出”字样
	HBITMAP hbmScene;
	HBITMAP hbmBkg;
	HBITMAP hbmHumanCards;
	HBITMAP hbmCards;
	HBITMAP hbmCardBack;
	HBITMAP hbmNoDiscard;
	HBRUSH hbrush;
	SIZE sceneSize;
	SIZE cardSize;
	
public:
	HWND no, score1, score2, score3, discand, pass, hint;
};