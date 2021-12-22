#pragma once

typedef struct sKeyState {
	bool bPressed;
	bool bReleased;
	bool bHeld;
}sKeyState;

void KeyUpdate();
sKeyState GetKey(int nKeyID);
sKeyState GetMouse(int nMouseButtonID);