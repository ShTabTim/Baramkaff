#include "FundLibs/Win/win.h"
#include <glad/glad.h>
#include "FundLibs/HelperGL/HelperGL.h"
#include "FundLibs/HelperGL/Shader.h"
#include <thread>

winClass wc;
hWindow helpWin, gWin;
hButton button;
hScrollBar scroll;
bool isLive = true;
float hg = 0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		//if (hWnd == gWin.getWin()) {
			PostQuitMessage(0);
			isLive = false;
		//}
		break;
	case WM_SIZE:
		gWin.resize();
		helpWin.resize();
		break;
	case WM_COMMAND:
		if (button.down(lParam))
			TextOut(helpWin.getHDC(), 10, 120, L" Ќќѕќ„ ј!!!", 11);
		break;
	case WM_HSCROLL:
		scroll.update(wParam);
		hg = ((float)(scroll.getPos())) / 100.0f;
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			isLive = false;
			break;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

float vertices[] = {
	// координаты       // цвета
	0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // нижн€€ права€ вершина
   -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // нижн€€ лева€ вершина
	0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // верхн€€ вершина
};
GLuint vbo, vao;

void rend() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, 3);

}
void rendThread() {
	HGLRC hRC;
	HDC hdc;
	EnableOpenGL(gWin, &hdc, &hRC);
	gladLoadGL();

	program prog;
	prog.setShaderFVG("Shaders/main.vert.glsl", 0);
	prog.setShaderFVG("Shaders/main.frag.glsl", 1);
	prog.setShaderFVG("Shaders/main.geom.glsl", 2);
	prog.create();
	prog.use();

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//  оординатные артибуты
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ÷ветовые атрибуты
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	do {
		rend();
		SwapBuffers(hdc);
	} while (isLive);
	DisableOpenGL(gWin, hdc, hRC);
}

int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow) {
	wc.init(hInst, pCmdLine, nCmdShow, L"Baramkaff", WndProc);

	gWin.setXY(CW_USEDEFAULT, CW_USEDEFAULT);
	gWin.setWH(256, 256);
	gWin.init(&wc, L"Graphical panel", WS_OVERLAPPEDWINDOW, nullptr);
	gWin.show(false);

	helpWin.setXY(CW_USEDEFAULT, CW_USEDEFAULT);
	helpWin.setWH(512, 128);
	helpWin.init(&wc, L"Help panel", WS_OVERLAPPEDWINDOW, gWin);
	helpWin.show(false);
	
	button.setXY(10, 10);
	button.setWH(100, 30);
	button.init(L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, helpWin);

	scroll.setXY(10, 42);
	scroll.setWH(400, 30);
	scroll.init(WS_CHILD | WS_VISIBLE | SBS_HORZ, helpWin);
	scroll.setScrollRange(0, 100);
	scroll.setPos(20);

	std::thread rTh(rendThread);

	MSG msg;
	do if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} while (msg.message != WM_QUIT);
	rTh.join();
	return msg.wParam;
}
