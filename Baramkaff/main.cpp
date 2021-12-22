#include "FundLibs/BarakWinHelper/Barak.h"
#include "FundLibs/BarakWinHelper/Win/Keys.h"
#include "FundLibs/ObjectsGL/Shader.h"

//glad 4.6

void prepr(hWindow* g_Win){
	g_Win->setXY(CW_USEDEFAULT, CW_USEDEFAULT);
	g_Win->setWH(512, 512);
	g_Win->rename(L"Graphical panel");
}

int main() {

	//program prog;
	//prog.setShaderFVG("Shaders/main.vert.glsl", 0);
	//prog.setShaderFVG("Shaders/main.frag.glsl", 1);
	//prog.create();
	//prog.use();

	pipeprog prog;
	prog.gen();
	prog.create(GL_VERTEX_SHADER, GL_VERTEX_SHADER_BIT, "Shaders/main.vert.glsl");
	prog.create(GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER_BIT, "Shaders/main.frag.glsl");
	prog.bind();

	char* vel;

	while (threadIsLive()) {
		KeyUpdate();
		vel = new char[] { 0, 0, 0 };
		if (GetKey('W').bHeld)
			vel[0] += 1;
		if (GetKey('A').bHeld)
			vel[2] -= 1;
		if (GetKey('S').bHeld)
			vel[0] -= 1;
		if (GetKey('D').bHeld)
			vel[2] += 1;
		if (GetKey(VK_SHIFT).bHeld)
			vel[1] -= 1;
		if (GetKey(VK_SPACE).bHeld)
			vel[1] += 1;

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColorRGB(0, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, 3);


		SwapBuffers();
	}
	return 99;
}