#version 450 core
#line 1 0
layout(std140, binding=0) uniform inputs {
    vec4 speedCam;
    vec4 posCam;
    vec4 angCam;

    vec4 IMG;
    vec4 WLD_SIZ;

    vec4 seed;
};
layout(std430, binding=0) readonly buffer voxels_block {
    uint voxels[];
};
layout(std430, binding = 1) buffer image_block {
    vec4 image[];
};
#line 1 1

#define MaxRenderL 1024
#define MaxIterL 128

float gfh = 100;
float modFGH(float x){return x - floor(x / gfh) * gfh;}
vec4  modFGH(vec4  x){return x - floor(x / gfh) * gfh;}
vec4 perm(vec4 x){return modFGH(((x * 34.0) + 1.0) * x);}


uvec4 R_STATE;

uint TausStep(uint z, int S1, int S2, int S3, uint M)
{
	uint b = (((z << S1) ^ z) >> S2);
	return (((z & M) << S3) ^ b);	
}

uint LCGStep(uint z, uint A, uint C)
{
	return (A * z + C);	
}

vec2 hash22(vec2 p)
{
	p += seed.x;
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xx+p3.yz)*p3.zy);
}

float random() {
	R_STATE.x = TausStep(R_STATE.x, 13, 19, 12, uint(4294967294));
	R_STATE.y = TausStep(R_STATE.y, 2, 25, 4, uint(4294967288));
	R_STATE.z = TausStep(R_STATE.z, 3, 11, 17, uint(4294967280));
	R_STATE.w = LCGStep(R_STATE.w, uint(1664525), uint(1013904223));
	return 2.3283064365387e-10 * float((R_STATE.x ^ R_STATE.y ^ R_STATE.z ^ R_STATE.w));
}

vec3 randomOnSphere() {
    return normalize(vec3(random()+random(), random()+random(), random()+random()) - 1);
}


float noise(vec3 p){
    p *= 10;
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 / 41);
    vec4 o2 = fract(k4 / 41);

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

float octave_noise(int octaves, vec3 pos){
    float gh;
    for(int j = 0; j < octaves; j++)
        gh += noise(pos*j);
    return gh/octaves;
}

vec2 cube_intersect(vec3 ro, vec3 rd, float size) {
    ro = -ro;
    vec3 t0 = ro / rd;
    vec3 t1 = (vec3(size)+ro) / rd;
    vec3 t2 = min(t0, t1);
    vec3 t3 = max(t0, t1);
    return vec2(max(max(t2.x, t2.y), t2.z), min(min(t3.x, t3.y), t3.z));
}

vec2 cube_intersect(vec3 ro, vec3 rd, float size, out vec3 norm) {
    ro = -ro;
    vec3 t0 = ro / rd;
    vec3 t1 = (vec3(size)+ro) / rd;
    vec3 t2 = min(t0, t1);
    vec3 t3 = max(t0, t1);
    float tN = max(max(t2.x, t2.y), t2.z);
    float tF = min(min(t3.x, t3.y), t3.z);
    if (tN > tF || tF < 0) return vec2(-1);
    norm = -sign(rd) * step(t2.yzx, t2) * step(t2.zxy, t1);
    return vec2(tN, tF);
}

vec2 sphere_intersect(vec3 ro, vec3 rd, float r) {
    float b = -dot(ro, rd);
    float h = b * b - dot(ro, ro) + r * r;
    if (h < 0) return vec2(-1);
    h = sqrt(h);
    return vec2(b-h, b+h);
}

float plat_intersect(vec3 ro, vec3 rd, vec3 n) {
	return -dot(ro, n) / dot(rd, n);
}

uint getVoxel(vec3 p) {
    if (p.x < WLD_SIZ.x && p.y < WLD_SIZ.y && p.z < WLD_SIZ.z && p.x >= 0 && p.y >= 0 && p.z >= 0)
        return voxels[uint(p.x*WLD_SIZ.y*WLD_SIZ.z+p.y*WLD_SIZ.z+p.z)];
}

uint vox_inter(vec3 ro, vec3 rd, inout vec3 vpos, inout vec3 norm) {
    vpos = floor(ro);
    
    vec3 step = sign(rd);
    vec3 tDelta = step / rd;

    
    float tMaxX, tMaxY, tMaxZ;
    
    vec3 fr = fract(ro);
    
    tMaxX = tDelta.x * ((rd.x>0.0) ? (1.0 - fr.x) : fr.x);
    tMaxY = tDelta.y * ((rd.y>0.0) ? (1.0 - fr.y) : fr.y);
    tMaxZ = tDelta.z * ((rd.z>0.0) ? (1.0 - fr.z) : fr.z);

    for (int i = 0; i < MaxIterL; i++) {
        uint h = getVoxel(ivec3(vpos));
        if (h != 0) {
            return h;
        }
        
        if (tMaxX < tMaxY) {
            if (tMaxZ < tMaxX) {
                tMaxZ += tDelta.z;
                vpos.z += step.z;
                norm = vec3(0, 0,-step.z);
            } else {
                tMaxX += tDelta.x;
            	vpos.x += step.x;
                norm = vec3(-step.x, 0, 0);
            }
        } else {
            if (tMaxZ < tMaxY) {
                tMaxZ += tDelta.z;
                vpos.z += step.z;
                norm = vec3(0, 0, -step.z);
            } else {
            	tMaxY += tDelta.y;
            	vpos.y += step.y;
                norm = vec3(0, -step.y, 0);
            }
        }
    }

 	return 0;
}

#define _mirr_ 0
#define _diff_ 1
#define _glos_ 2
#define _ligh_ 3
#define SkyCol vec3(0.5, 0.7, 0.8)
#define UnderSkyCol vec3(0.01, 0.1, 0.2)
#define sky 100
#define K3 1.73205080756887729
#define K3on2 0.8660254037844386

struct body {
    vec3 norm;
    vec3 col;
    float lMax;
    uint mat;
    float glossy;
    float bright;
};

body intersect(vec3 ro, vec3 rd) {
    body bod;
    bod.lMax = MaxRenderL;
    bod.mat = _mirr_;
    bod.bright = 0.8;

    vec3 norm, vpos;
    float lll = plat_intersect(vec3(0, -WLD_SIZ.y -sky + ro.y, 0), rd, vec3(0, -1, 0));
    
    if (lll > 0 && lll < bod.lMax){
        bod.lMax = lll;
        bod.norm = normalize(vec3(0, -1, 0));
        bod.col = SkyCol;
        bod.mat = _ligh_;
    }

    lll = sphere_intersect(vec3(-WLD_SIZ.x/2+ro.x, -WLD_SIZ.y -sky+ro.y, -WLD_SIZ.z/2+ro.z), rd, 100).x;
    if (lll > 0 && lll < bod.lMax){
        bod.lMax = lll;
        bod.norm = normalize(ro + rd * lll - vec3( -WLD_SIZ.x/2+ro.x, -WLD_SIZ.y -64+ro.y, -WLD_SIZ.z/2+ro.z));
        bod.col = vec3(1.1, 1.1, 0.9)*3;
        bod.mat = _ligh_;
    }

    uint id = vox_inter(ro, rd, vpos, norm);

    if (id == 0) 
        return bod;

    lll = cube_intersect(ro - vec3(vpos), rd, 1).x;
    if (lll > 0 && lll < bod.lMax){
        bod.lMax = lll;
        bod.norm = norm;

        vec3 fr = ro + rd*(lll.x-0.001f);
        switch (id) {
        case 1:
            bod.col = vec3(1, 0.8, 0.4);
            vec3 ght = floor(fr*16.0)/128;
            bod.col *= 0.5 + 0.5*noise(vec3(noise(ght.xyz), noise(ght.yzx), noise(ght.zxy)));
            bod.mat = _ligh_;
            break;
        case 2:
            bod.col = vec3(0, 1, 0);
            bod.col *= 0.5 + 0.5*noise(floor(fr*8)/8);
            bod.mat = _glos_;
            bod.bright = 0.5;
            bod.glossy = 5;
            break;
        case 3:
            bod.col = vec3(1);
            bod.mat = _diff_;
            break;
        case 4:
            bod.col = vec3(0, 0, 1);
            bod.col *= 0.5 + 0.5*noise(fr/8);
            bod.mat = _glos_;
            bod.glossy = 2;
            break;
        case 5:
            bod.col = vec3(1, 1, 1);
            bod.mat = _mirr_;
            bod.bright = 0.6;
            break;
        case 6:
            bod.col = vec3(1, 0, 0);
            bod.mat = _mirr_;
            bod.bright = 0.6;
            bod.col *= 0.7+0.3*octave_noise(2,fr*8);
            break;
        case 7:
            bod.col = vec3(0.5);
            bod.mat = _diff_;
            bod.col.g += 0.2 + 0.2*octave_noise(2,ivec3(fr*8));
            break;
        default:
            bod.col = vec3(0.1);
            bod.mat = _diff_;
            break;
        }
    }
    
    return bod;
}

vec3 castRay(inout vec3 ro, inout vec3 rd, inout int iter) {
    body bod = intersect(ro, rd);
    if (iter == 1) {
        iter = -1;
        return vec3(0.01);
    }
    if (bod.lMax == MaxRenderL) { 
        vec3 cal = UnderSkyCol*float(iter)/3;
        iter = -1;
        return cal;
    } else {
        iter--;
        switch (bod.mat) {
        case _ligh_:
            iter = -1;
            return bod.col;
            break;
        case _mirr_:
            ro += rd * (bod.lMax - 0.001);
            rd = normalize(rd - bod.norm * 2 * dot(rd, bod.norm));
            return bod.col * bod.bright;
            break;
        case _diff_:
            ro = ro + rd * (bod.lMax - 0.001);
            vec3 randTAn = randomOnSphere() + bod.norm*0.01;
            randTAn = normalize(randTAn);
            rd = randTAn; 
            return bod.col * bod.bright;
            break;
        case _glos_:
            ro += rd * (bod.lMax - 0.001);
            vec3 ref = rd - bod.norm * 2 * dot(rd, bod.norm);
            vec3 randTAngl = randomOnSphere();
            float gloss = dot(ref, bod.norm);
            randTAngl = normalize(randTAngl / dot(ref, randTAngl)) / bod.glossy + ref * (sqrt(1 - gloss * gloss) + K3on2) / gloss;
            rd = normalize(randTAngl); 
            return bod.col * bod.bright;
            break;
        }
    }    
    return bod.col;
}

vec3 traceRay(vec3 ro, vec3 rd, int iter){
    vec3 color = vec3(1);
    int i = iter;
    while (i > 0)
        color *= castRay(ro, rd, i);
    return color;
}


out vec4 FragColor;
in vec2 FragCord;

void main() {
	vec2 uvRes = hash22(FragCord + 1)*IMG.xy + IMG.xy;
    R_STATE = ivec4(seed.xyxy + uvRes.xxyy);
    vec2 s = sin(angCam.xy);
    vec2 c = cos(angCam.xy);
    vec2 scrtowld = vec2(FragCord.x*IMG.x/IMG.y, FragCord.y);
    vec3 rd = normalize(vec3(scrtowld*0.1, 1) + vec3(scrtowld, 0));

    float rddy = rd.y;
    rd.y = c.y*rd.y + s.y*rd.z;
    rd.z =-s.y*rddy + c.y*rd.z;

    rddy = rd.x;
    rd.x = c.x*rd.x + s.x*rd.z;
    rd.z =-s.x*rddy + c.x*rd.z;
    vec2 scr = (FragCord + 1)*0.5*IMG.xy;
    
    int indexx = int(IMG.y*scr.x + scr.y);
    image[indexx] = mix(image[indexx], vec4(traceRay(posCam.xyz, rd, 6), 1), min(1, 0.01 + speedCam.x));
    FragColor = image[indexx];

}