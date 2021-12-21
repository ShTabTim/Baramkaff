#pragma once
#include "../Win/win.h"

void EnableOpenGL(hWindow w, HDC* hDC, HGLRC* hRC);
void DisableOpenGL(hWindow w, HDC hDC, HGLRC hRC);