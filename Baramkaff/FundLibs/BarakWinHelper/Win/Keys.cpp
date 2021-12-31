#include "Keys.h"
#include <Windows.h>

short m_keyOldState[256] = { 0 };
short m_keyNewState[256] = { 0 };
sKeyState m_keys[256], m_mouse[5];	
POINT pt = { 0, 0 };
POINT ptOld = { 0, 0 };


bool m_mouseOldState[5] = { 0 };
bool m_mouseNewState[5] = { 0 };
sKeyState GetKey(int nKeyID) { return m_keys[nKeyID]; }
sKeyState GetMouse(int nMouseButtonID) { return m_mouse[nMouseButtonID]; }
void ssetXY(int x, int y) { pt.x = x; pt.y = y; }
int ggetX() { return pt.x; }
int ggetY() { return pt.y; }
int getdx() { return pt.x-ptOld.x; }
int getdy() { return pt.y-ptOld.y; }

void KeyUpdate(int x, int y) {
	for (int i = 0; i < 256; i++) {
		m_keyNewState[i] = GetAsyncKeyState(i);

		m_keys[i].bPressed = false;
		m_keys[i].bReleased = false;

		if (m_keyNewState[i] != m_keyOldState[i])
			if (m_keyNewState[i] & 0x8000) {
				m_keys[i].bPressed = !m_keys[i].bHeld;
				m_keys[i].bHeld = true;
			}
			else {
				m_keys[i].bReleased = true;
				m_keys[i].bHeld = false;
			}

		m_keyOldState[i] = m_keyNewState[i];
	}
	m_mouse[0] = m_keys[VK_LBUTTON ];
	m_mouse[1] = m_keys[VK_RBUTTON ];
	m_mouse[2] = m_keys[VK_MBUTTON ];
	m_mouse[3] = m_keys[VK_XBUTTON1];
	m_mouse[4] = m_keys[VK_XBUTTON2];
	ptOld = pt;
	GetCursorPos(&pt);
	pt.x -= x;
	pt.y -= y;
}
