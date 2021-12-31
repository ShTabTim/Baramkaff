#include "FundLibs/BarakWinHelper/Barak.h"
#include "FundLibs/BarakWinHelper/Win/Keys.h"
#include "FundLibs/ObjectsGL/Shader.h"
#include "Noises/Simplex.h"
#include <chrono>
#include <vector>
#include <random>

//glad 4.6
SimplexNoise noi;
typedef GLuint vox;

float ff(int g) { return (float)g; }

#define NoisSize 2.0f
#define FixedRad 32
#define RandH 32

vox getVoxel(int x, int y, int z) {
	float Rad = sqrt(x*x+y*y+z*z);
	if (Rad < FixedRad + RandH*(1+pow(abs(noi.noise(NoisSize*x/Rad, NoisSize*y/Rad, NoisSize*z/Rad)), 3))*0.5f)
		return 3;// +1 * noi.noise(x / 16.0f, y / 16.0f, z / 16.0f);
	else
		return 0;
}
typedef size_t uint;
typedef struct int3{
	int x, y, z;
	int3() {}
	int3(int x, int y, int z) :x(x), y(y), z(z) {}
}int3;
template <typename T>
int3 toint3(T* d) { return int3(d[0], d[1], d[2]); }
typedef struct int2{
	int x, y;
	int2() {}
	int2(int x, int y) :x(x), y(y) {}
}int2;

typedef struct float3{
	float x, y, z;
	float3() {}
	float3(float x, float y, float z) :x(x), y(y), z(z) {}
}float3;
float3 tofloat3(float* d) { return float3(d[0], d[1], d[2]); }
typedef struct float2{
	float x, y;
	float2() {}
	float2(float x, float y) :x(x), y(y) {}
}float2;
	float2 tofloat2(float* d) { return float2(d[0], d[1]); }

int3 vox_inter(float3 ro, float3 rd, float3* norm, vox* voxels, int3 size_wld, int3* oldpos) {
	int3 vpos = int3(ro.x, ro.y, ro.z);
	*oldpos = vpos;

	float3 step = float3((rd.x>0)-(rd.x<0), (rd.y>0)-(rd.y<0), (rd.z>0)-(rd.z<0));
	float3 tDelta = float3(step.x/rd.x, step.y/rd.y, step.z/rd.z);

	float tMaxX, tMaxY, tMaxZ;

	float3 fr = float3(ro.x-int(ro.x),ro.y-int(ro.y),ro.z-int(ro.z));

	tMaxX = tDelta.x * ((rd.x > 0.0) ? (1.0 - fr.x) : fr.x);
	tMaxY = tDelta.y * ((rd.y > 0.0) ? (1.0 - fr.y) : fr.y);
	tMaxZ = tDelta.z * ((rd.z > 0.0) ? (1.0 - fr.z) : fr.z);

	for (int i = 0; i < 64; i++) {
		if (vpos.x < size_wld.x && vpos.x >= 0 &&
			vpos.y < size_wld.y && vpos.y >= 0 &&
			vpos.z < size_wld.z && vpos.z >= 0)
			if (voxels[vpos.x*size_wld.y*size_wld.z + vpos.y*size_wld.z + vpos.z] != 0) {
				return vpos;
			}
		*oldpos = vpos;

		if (tMaxX < tMaxY) {
			if (tMaxZ < tMaxX) {
				tMaxZ += tDelta.z;
				vpos.z += step.z;
				*norm = float3(0, 0, -step.z);
			}
			else {
				tMaxX += tDelta.x;
				vpos.x += step.x;
				*norm = float3(-step.x, 0, 0);
			}
		}
		else {
			if (tMaxZ < tMaxY) {
				tMaxZ += tDelta.z;
				vpos.z += step.z;
				*norm = float3(0, 0, -step.z);
			}
			else {
				tMaxY += tDelta.y;
				vpos.y += step.y;
				*norm = float3(0, -step.y, 0);
			}
		}
	}

	*norm = float3(0, 0, 0);
	return int3(0,0,0);
}
int3 getIntersect(int2 mousePos, int2 size_scr, float3 pos, float2 angles, vox* voxels, int3 size_wld, float3* norm, int3* oldpos) {
	float2 s = float2(sin(angles.x), sin(angles.y));
	float2 c = float2(cos(angles.x), cos(angles.y));
	float2 ScrCord = float2(ff(2*mousePos.x-size_scr.x)/ff(size_scr.y), 1-ff(2*mousePos.y)/ff(size_scr.y));
	float3 rd = float3(ScrCord.x*0.1, ScrCord.y*0.1, 1);
	rd.x += ScrCord.x;
	rd.y += ScrCord.y;
	
    float rddy = rd.y;
    rd.y = c.y*rd.y + s.y*rd.z;
    rd.z =-s.y*rddy + c.y*rd.z;

    rddy = rd.x;
    rd.x = c.x*rd.x + s.x*rd.z;
    rd.z =-s.x*rddy + c.x*rd.z;

	float3 ro = pos;

	return vox_inter(ro, rd, norm, voxels, size_wld, oldpos);
}

void prepr(hWindow* g_Win){
	g_Win->rename(L"Graphical panel");
	g_Win->setWH(512, 512);
}

int main() {
	srand(time(0));

	pipeprog rend;
	rend.gen(2);
	rend.create(GL_VERTEX_SHADER,   GL_VERTEX_SHADER_BIT,   "Shaders/main.vert.glsl", 0);
	rend.create(GL_FRAGMENT_SHADER,   GL_FRAGMENT_SHADER_BIT,   "Shaders/main.frag.glsl", 0);
	rend.bind();

	struct UnIn {
		GLfloat speedCam[4];
		GLfloat posCam[4];
		GLfloat angCam[4];

		GLfloat IMG[4];
		GLfloat WLD_SIZE[4];

		GLfloat seed[4];
	} inPPut;

	inPPut.WLD_SIZE[0] = 128;
	inPPut.WLD_SIZE[1] = 256;
	inPPut.WLD_SIZE[2] = 128;

	vox* voxels = new vox[inPPut.WLD_SIZE[0]*inPPut.WLD_SIZE[1]*inPPut.WLD_SIZE[2]];

	for (size_t i = 0; i < inPPut.WLD_SIZE[0]; i++)
		for (size_t j = 0; j < inPPut.WLD_SIZE[1]; j++)
			for (size_t k = 0; k < inPPut.WLD_SIZE[2]; k++) {
				voxels[(uint)(i*inPPut.WLD_SIZE[1]*inPPut.WLD_SIZE[2]+j*inPPut.WLD_SIZE[2]+k)] = getVoxel(i-inPPut.WLD_SIZE[0]*0.5, j-inPPut.WLD_SIZE[1]*0.5, k-inPPut.WLD_SIZE[2]*0.5);
			}

	GLuint voxels_buffer;
	glGenBuffers(1, &voxels_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxels_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vox)*inPPut.WLD_SIZE[0]*inPPut.WLD_SIZE[1]*inPPut.WLD_SIZE[2], voxels, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxels_buffer);

	inPPut.IMG[0] = getWind().getW();
	inPPut.IMG[1] = getWind().getH();
	GLuint image_buffer;
	glGenBuffers(1, &image_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, image_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 4 * inPPut.IMG[0]*inPPut.IMG[1], NULL, GL_DYNAMIC_DRAW);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	float speed = 10;
	float angSpeed = 1.5;
	char vel[3];
	inPPut.posCam[0] = inPPut.WLD_SIZE[0] / 2;
	inPPut.posCam[1] = inPPut.WLD_SIZE[1] / 2 + FixedRad + RandH + 5;
	inPPut.posCam[2] = inPPut.WLD_SIZE[2] / 2;
	char angVel[2] = { 0, 0 };
	inPPut.angCam[0] = 0;
	inPPut.angCam[1] = 0;
	inPPut.speedCam[0] = 1;
	float spdNow = 0;

	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UnIn), &inPPut, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

	float dt;
	std::chrono::system_clock::time_point tp1 = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point tp2 = std::chrono::system_clock::now();
	std::chrono::duration<float> buildddt(0);
	std::chrono::duration<float> buildTime(0.15);
	bool gh = true;
	bool isDo = false;
	bool update = false;

	vox chooseVoleBuild = 1;

	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<> dist(0.0f, 1.0f);

	while (threadIsLive()) {
		{
			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> dTime = tp2 - tp1;
			tp1 = tp2;
			dt = dTime.count();
			if (buildddt.count() > 0)
				buildddt -= dTime;
		}
		if (update) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxels_buffer);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(vox)*inPPut.WLD_SIZE[0]*inPPut.WLD_SIZE[1]*inPPut.WLD_SIZE[2], voxels);
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			update = false;
			inPPut.speedCam[0] = 0.1;
		}
		RECT rcClient, rcWind;
		GetClientRect(getWind().getWin(), &rcClient);
		GetWindowRect(getWind().getWin(), &rcWind);
		int border_thickness_x = ((rcWind.right - rcWind.left) - rcClient.right) / 2;
		int border_thickness_y = (rcWind.bottom - rcWind.top) - rcClient.bottom - border_thickness_x;
		KeyUpdate(getWind().getX() + border_thickness_x, getWind().getY() + border_thickness_y);
		if (GetKey(VK_ESCAPE).bHeld)
			return 1;

		if (GetKey('1').bPressed)
			chooseVoleBuild = 1;
		if (GetKey('2').bPressed)
			chooseVoleBuild = 2;
		if (GetKey('3').bPressed)
			chooseVoleBuild = 3;
		if (GetKey('4').bPressed)
			chooseVoleBuild = 4;
		if (GetKey('5').bPressed)
			chooseVoleBuild = 5;
		if (GetKey('6').bPressed)
			chooseVoleBuild = 6;
		if (GetKey('7').bPressed)
			chooseVoleBuild = 7;
		if (GetKey('8').bPressed)
			chooseVoleBuild = 8;
		if (GetKey('9').bPressed)
			chooseVoleBuild = 9;

		if ((GetMouse(0x00).bHeld || GetMouse(0x01).bHeld) && buildddt.count() <= 0) {
			float3 norm;
			int3 oldpopo;
			int3 popo = getIntersect(int2(ggetX(), ggetY()), int2(getWind().getW(), getWind().getH()), tofloat3(inPPut.posCam), tofloat2(inPPut.angCam), voxels, toint3(inPPut.WLD_SIZE), &norm, &oldpopo);
			if (GetMouse(0x01).bHeld) popo = oldpopo;
			if (norm.x != 0 || norm.y != 0 || norm.z != 0) {
				voxels[(uint)((popo.x*inPPut.WLD_SIZE[1]+popo.y)*inPPut.WLD_SIZE[2]+popo.z)] = GetMouse(0x01).bHeld ? chooseVoleBuild : 0;
				update = true;
				buildddt = buildTime;
			}
		}
		if (GetMouse(0x00).bReleased || GetMouse(0x01).bReleased)
			buildddt = std::chrono::duration<float>(0);
		spdNow = 0;
		angVel[0] = 0; isDo = false; 
		if (GetKey(VK_LEFT ).bHeld)
			angVel[0] -= ((isDo ^= true), 1);
		if (GetKey(VK_RIGHT).bHeld)
			angVel[0] += ((isDo ^= true), 1);
		if (isDo) spdNow += 1;
		inPPut.angCam[0] += ((float)angVel[0]) * angSpeed * dt;
		if (inPPut.angCam[0] > 3.14159265f)
			inPPut.angCam[0] -= 6.2832853f;
		if (inPPut.angCam[0] < -3.14159265f)
			inPPut.angCam[0] += 6.2832853f;

		angVel[1] = 0; isDo = false;
		if ((GetKey(VK_UP  ).bHeld) && inPPut.angCam[1] < 1.57079632f)
			angVel[1] += ((isDo ^= true), 1);
		if ((GetKey(VK_DOWN).bHeld) && inPPut.angCam[1] > -1.57079632f)
			angVel[1] -= ((isDo ^= true), 1);
		if (isDo) spdNow += 1;
		inPPut.angCam[1] += ((float)angVel[1]) * angSpeed * dt;

		isDo = false;
		int voxPosFut = 0;
		float futPos = 0;
		vel[0] = 0; 
		if (GetKey('A').bHeld)
			vel[0] -= 1;
		if (GetKey('D').bHeld)
			vel[0] += 1;

		vel[1] = 0;
		if (GetKey(VK_SHIFT).bHeld)
			vel[1] -= 1;
		if (GetKey(VK_SPACE).bHeld)
			vel[1] += 1;

		vel[2] = 0;
		if (GetKey('S').bHeld)
			vel[2] -= 1;
		if (GetKey('W').bHeld)
			vel[2] += 1;


		if (vel[2] != 0 || vel[0] != 0) {
			futPos = inPPut.posCam[0] + (((float)vel[0]) * cos(inPPut.angCam[0]) + ((float)vel[2]) * sin(inPPut.angCam[0])) * speed * dt;
			if (voxels[(uint)(((int)futPos) * inPPut.WLD_SIZE[1] * inPPut.WLD_SIZE[2] + ((int)(inPPut.posCam[1])) * inPPut.WLD_SIZE[2] + ((int)(inPPut.posCam[2])))] == 0)
				inPPut.posCam[0] = ((isDo |= true), futPos);
		}
		if (vel[1] != 0) {
			futPos = inPPut.posCam[1] + ((float)vel[1]) * speed * dt;
			if (voxels[(uint)(((int)(inPPut.posCam[0])) * inPPut.WLD_SIZE[1] * inPPut.WLD_SIZE[2] + ((int)futPos) * inPPut.WLD_SIZE[2] + ((int)(inPPut.posCam[2])))] == 0)
				inPPut.posCam[1] = ((isDo |= true), futPos);
		}
		if (vel[2] != 0 || vel[0] != 0) {
			futPos = inPPut.posCam[2] + (((float)vel[2]) * cos(inPPut.angCam[0]) - ((float)vel[0]) * sin(inPPut.angCam[0])) * speed * dt;
			if (voxels[(uint)(((int)(inPPut.posCam[0])) * inPPut.WLD_SIZE[1] * inPPut.WLD_SIZE[2] + ((int)(inPPut.posCam[1])) * inPPut.WLD_SIZE[2] + ((int)futPos))] == 0)
				inPPut.posCam[2] = ((isDo |= true), futPos);
		}

		if (isDo) spdNow += 1;

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0, 0, 0, 0);

		if (spdNow == 0 && inPPut.speedCam[0] > 0) inPPut.speedCam[0] -= 0.1*inPPut.speedCam[0];
		else if (spdNow != 0) inPPut.speedCam[0] = spdNow;
		else if (inPPut.speedCam[0] < 0) inPPut.speedCam[0] = 0;

		inPPut.IMG[0] = getWind().getW();
		inPPut.IMG[1] = getWind().getH();

		inPPut.seed[0] = dist(e2)*999.0f;
		inPPut.seed[1] = dist(e2)*999.0f;
		inPPut.seed[2] = dist(e2)*999.0f;
		inPPut.seed[3] = dist(e2)*999.0f;

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UnIn), &inPPut);

		rend.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		SwapBuffers();
	}
	return 99;
}