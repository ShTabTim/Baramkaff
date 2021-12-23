#version 450 core

uniform vec3 pos;
uniform vec2 angles;
out vec4 FragColor;
in vec2 FragCord;

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    p *= 10;
    //p = p - ivec3(p);
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

vec2 cube_intersect(vec3 ro, vec3 rd, float rad, out vec3 outNormal) {
    ro = -ro;
    vec3 t0 = ro / rd;
    vec3 t1 = (ro + vec3(rad)) / rd;
    vec3 t2 = min(t0, t1);
    vec3 t3 = max(t0, t1);
    vec2 l = vec2(max(max(t2.x, t2.y), t2.z), min(min(t3.x, t3.y), t3.z));
    if (l.x > l.y || l.y < 0.0) return vec2(-1.0);
    outNormal = -sign(rd) * step(t2.yzx, t2) * step(t2.zxy, t2);
    return l;
}
vec2 cube_intersect(vec3 ro, vec3 rd, float rad) {
    ro = -ro;
    vec3 t0 = ro / rd;
    vec3 t1 = (ro + vec3(rad)) / rd;
    vec3 t2 = min(t0, t1);
    vec3 t3 = max(t0, t1);
    vec2 l = vec2(max(max(t2.x, t2.y), t2.z), min(min(t3.x, t3.y), t3.z));
    if (l.x > l.y || l.y < 0.0) return vec2(-1.0);
    return l;
}

#define MaxRenderL 1024

struct body {
    vec3 norm;
    vec3 color;
    float lMax;
    bool isLight;
};

void intersect(vec3 ro, vec3 rd, out body bod) {
    float l = MaxRenderL;
    bod.lMax = MaxRenderL;
    vec3 norm;

    l = cube_intersect(ro - vec3(0, 0, 3), rd, 4, norm).x;
    if (l > 0 && l < bod.lMax) {
        bod.color = vec3(0.8, 0.8, 0.2);
        bod.norm = norm;
        bod.lMax = l;
    }

    l = cube_intersect(ro - vec3(4, 2, 2), rd, 4, norm).x;
    if (l > 0 && l < bod.lMax) {
        bod.color = vec3(0.8, 0.2, 0.8);
        bod.norm = norm;
        bod.lMax = l;
    }

    l = cube_intersect(ro - vec3(5, 1, 1), rd, 2, norm).x;;
    if (l > 0 && l < bod.lMax) {
        bod.norm = norm;
        bod.color = normalize(ro + rd * l - vec3(5, 1, 1));
        bod.lMax = l;
        bod.isLight = true;
    }
}

vec3 castRay(inout vec3 ro, inout vec3 rd, out body bod) {
    bod.lMax = MaxRenderL;
    bod.isLight = false;

    intersect(ro, rd, bod);

    if (bod.lMax == MaxRenderL)
        return vec3(0);
    else {
        ro += rd * (bod.lMax - 0.001f);
        rd = normalize(rd - bod.norm * 2 * dot(rd, bod.norm));
        bod.color *= noise(vec3(noise(ro.xyz), noise(ro.yzx), noise(ro.zxy)));
    }
    return bod.color;
}
vec3 pow(vec3 g, float h) {
    g.x = pow(g.x, h);
    g.y = pow(g.y, h);
    g.z = pow(g.z, h);
    return g;
}
void main() {

    vec2 s = sin(angles);
    vec2 c = cos(angles);
    vec3 rd = normalize(vec3(FragCord*0, 1) + vec3(FragCord, 0)) 
            * 
            mat3( 1,   0,   0,
                  0,   c.y, s.y,
                  0,  -s.y, c.y )
            *
            mat3( c.x, 0,   s.x,
                  0,   1,   0,
                 -s.x, 0,   c.x );
    vec3 ro = pos;
    body bod;
    vec3 color = castRay(ro, rd, bod);
    vec3 lll = normalize(vec3(0.3, 0.5, -1));
    body gog;
    vec3 ll = castRay(ro, lll, gog);

    if (gog.lMax < MaxRenderL)
        bod.color /= 10;
    else
        bod.color *= dot(bod.norm, lll);

    FragColor = vec4(bod.color, 1);

}