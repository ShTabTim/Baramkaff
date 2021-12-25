#version 450 core

#define USE_SHADOWS false
#define MaxRenderL 256

layout(std430, binding=0) readonly buffer voxels_block {
    uint voxels[];
};

layout(location=1) uniform uint XX;
layout(location=2) uniform uint YY;
layout(location=3) uniform uint ZZ;
layout(location=4) uniform vec3 pos;
layout(location=5) uniform vec2 angles;
out vec4 FragColor;
in vec2 FragCord;

float gfh = 300;
float modFGH(float x){return x - floor(x / gfh) * gfh;}
vec4  modFGH(vec4  x){return x - floor(x / gfh) * gfh;}
vec4 perm(vec4 x){return modFGH(((x * 34.0) + 1.0) * x);}

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

uint getVoxel(vec3 p) {
    if (p.x < XX && p.y < YY && p.z < ZZ && p.x >= 0 && p.y >= 0 && p.z >= 0)
        return voxels[uint(p.x*YY*ZZ+p.y*ZZ+p.z)];
    //if (p.y < cos(p.x*3.14159265/8) * cos(p.z*3.14159265/8) * 0.1) {
    //    return voxels[uint(fract((p.x*8*8+p.y*8+p.z)/(8*8*8))*8*8*8)];
    //} else
    //    return 0;
}

vec3 getColor(uint id) {
    vec3 col;
    switch (id) {
    case 1:
        col = vec3(1, 0, 0);
        break;
    case 2:
        col = vec3(0, 1, 0);
        break;
    case 3:
        col = vec3(0, 0, 1);
        break;
    }
    return col;
}

vec3 vox_inter(vec3 ro, vec3 rd, inout vec3 vpos, inout vec3 norm) {
    vpos = floor(ro);
    
    vec3 step = sign(rd);
    vec3 tDelta = step / rd;

    
    float tMaxX, tMaxY, tMaxZ;
    
    vec3 fr = fract(ro);
    
    tMaxX = tDelta.x * ((rd.x>0.0) ? (1.0 - fr.x) : fr.x);
    tMaxY = tDelta.y * ((rd.y>0.0) ? (1.0 - fr.y) : fr.y);
    tMaxZ = tDelta.z * ((rd.z>0.0) ? (1.0 - fr.z) : fr.z);

    for (int i = 0; i < MaxRenderL; i++) {
        uint h = getVoxel(ivec3(vpos));
        if (h != 0) {
            return getColor(h);
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

 	return vec3(0,0,0);
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

    
    vec3 norm, vpos;
    vec3 col = vox_inter(ro - vec3(0, 0, 3), rd, vpos, norm);
    
    vec3 lightDir = normalize(vec3(-1.0, 3.0, -1.0));
    float diffuseAttn = max(dot(norm, lightDir), 0.0);
    vec3 light = vec3(1.0,0.9,0.9);
    
    vec3 ambient = vec3(0.2, 0.2, 0.3);
    
    vec3 reflected = reflect(rd, norm);
    float specularAttn = max(dot(reflected, lightDir), 0.0);

    col *= diffuseAttn*light*1.0 + specularAttn*light*0.6 + ambient;

    //vec3 fr = ro + rd * l;
    //col *= noise(vec3(noise(fr.xyz), noise(fr.yzx), noise(fr.zxy)));

    FragColor = vec4(col, 1);

}