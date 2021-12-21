#include "win.h"

//winClass//
winClass::winClass() {}
void winClass::init(HINSTANCE hInst, LPSTR lpCmdLine, int nCmdShow, LPCWSTR pClassName, WNDPROC fWmdProc) {
	this->hInst = hInst;
	this->lpCmdLine = lpCmdLine;
	this->nCmdShow = nCmdShow;
	this->pClassName = pClassName;

	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = fWmdProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = nullptr;
	wc.hCursor = GetCursor();
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;

	RegisterClassEx(&wc);
}
//winClass//

//hWind//
hWin::hWin() {}
hWin::hWin(std::nullptr_t) { hWnd = nullptr; }
HWND hWin::getWin() { return hWnd; }
void hWin::setXY(int x, int y) { this->x = x; this->y = y; }
void hWin::setWH(int w, int h) { this->h = h; this->w = w; }
size_t hWin::getW() { return w; }
size_t hWin::getH() { return h; }
//hWind//

//hWindow//
HDC hWindow::getHDC() { return hdc; }
void hWindow::init(winClass* wic, LPCWSTR name, DWORD dwStyle, hWin parent) {
	this->wic = wic;
	this->hWnd = CreateWindow(
		wic->pClassName,
		name,
		dwStyle,
		x, y, w, h,
		parent.getWin(), nullptr, wic->hInst, nullptr
	);
	hdc = GetDC(getWin());
}
void hWindow::show(bool Maximized) {
	RECT wRect, cRect;

	GetWindowRect(hWnd, &wRect);
	GetClientRect(hWnd, &cRect);

	wRect.right  += w - cRect.right - wRect.left;
	wRect.bottom += h - cRect.bottom - wRect.top;

	MoveWindow(hWnd, wRect.left, wRect.top, wRect.right, wRect.bottom, FALSE);

	ShowWindow(hWnd, Maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
}

void hWindow::resize() {
	RECT wRect;
	GetWindowRect(hWnd, &wRect);

	x = wRect.top;
	y = wRect.left;
	w = wRect.right - wRect.left;
	w = wRect.bottom - wRect.top;
}
//hWindow//

//hButton
void hButton::init(LPCWSTR name, DWORD dwStyle, hWin parent) {
	this->hWnd = CreateWindow(
		L"BUTTON",
		name,
		dwStyle,
		x,y,w,h,
		parent.getWin(), nullptr,
		(HINSTANCE)GetWindowLongPtr(parent.getWin(), GWLP_HINSTANCE),
		nullptr);
}
bool hButton::down(LPARAM lParam) { return lParam == (LPARAM)getWin(); }
//hButton//

//hScrollBar//
void hScrollBar::init(DWORD dwStyle, hWin parent) {
	hWnd = CreateWindow(L"SCROLLBAR", nullptr,
		dwStyle, 
		x, y, w, h,
		parent.getWin(), nullptr, 
		(HINSTANCE)GetWindowLongPtr(parent.getWin(), GWLP_HINSTANCE), 
		nullptr);
}
void hScrollBar::setScrollRange(size_t min, size_t max) { 
	this->min = min;
	this->max = max;
	SetScrollRange(getWin(), SB_CTL, min, max, true); 
}
void hScrollBar::setBigSpeed(size_t speed) { bigSpeed = speed; }
void hScrollBar::setPos(size_t pos) { 
	this->pos = pos; 
	SetScrollPos(getWin(), SB_CTL, this->pos, true);
}
size_t hScrollBar::getPos() { return pos; }
void hScrollBar::update(WPARAM wParam) {
	switch (LOWORD(wParam)) {
	case SB_PAGERIGHT:
		pos += bigSpeed;
		break;
	case SB_PAGELEFT:
		pos -= bigSpeed;
		break;
	case SB_LINERIGHT:
		pos++;
		break;
	case SB_LINELEFT:
		pos--;
		break;
	case SB_BOTTOM:
		pos = max;
		break;
	case SB_TOP:
		pos = min;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pos = HIWORD(wParam);
		break;
	}

	if (pos > max) pos = max; else if (pos < min) pos = min;

	SetScrollPos(getWin(), SB_CTL, pos, true);
}
//hScrollBar//