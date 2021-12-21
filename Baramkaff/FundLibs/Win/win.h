#pragma once
#include <Windows.h>

class winClass {
public:
	HINSTANCE hInst;
	LPSTR lpCmdLine;
	int nCmdShow;
	LPCWSTR pClassName;
	WNDCLASSEXW wc = { 0 };
	winClass();
	void init(HINSTANCE hInst, LPSTR lpCmdLine, int nCmdShow, LPCWSTR pClassName, WNDPROC fWndProc);
};

class hWin {
protected:
	HWND hWnd;
	int x, y;
	int w, h;
public:
	hWin();
	hWin(std::nullptr_t);
	HWND getWin();
	size_t getW();
	size_t getH();
	void setXY(int x, int y);
	void setWH(int w, int h);
};

class hWindow : public hWin{
private:
	winClass* wic;
	HDC hdc;
public:
	HDC getHDC();
	void init(winClass* wic, LPCWSTR name, DWORD dwStyle, hWin parent);
	void resize();
	void show(bool Maximized);
};

class hButton : public hWin{
public:
	void init(LPCWSTR name, DWORD dwStyle, hWin parent);
	bool down(LPARAM lParam);
};

class hScrollBar : public hWin{
private:
	size_t pos = 0;
	size_t min, max;
	size_t bigSpeed = 1;
public:
	void setBigSpeed(size_t speed);
	void setPos(size_t pos);
	void setScrollRange(size_t min, size_t max);
	size_t getPos();
	void update(WPARAM wParam);
	void init(DWORD dwStyle, hWin parent);
};