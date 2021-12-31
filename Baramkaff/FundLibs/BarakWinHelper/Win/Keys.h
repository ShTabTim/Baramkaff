#pragma once

typedef struct sKeyState {
	bool bPressed;
	bool bReleased;
	bool bHeld;
}sKeyState;

void KeyUpdate(int x, int y);
sKeyState GetKey(int nKeyID);
/*
* 0x00 => VK_LBUTTON 
* 0x01 => VK_RBUTTON 
* 0x02 => VK_MBUTTON 
* 0x03 => VK_XBUTTON1
* 0x04 => VK_XBUTTON2
*/
sKeyState GetMouse(int nMouseButtonID);
void ssetXY(int x, int y);
int ggetX();
int ggetY();
int getdx();
int getdy();