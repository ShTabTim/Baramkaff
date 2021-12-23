#version 450 core

uniform vec3 pos;
uniform vec2 angles;
out vec4 FragColor;
in vec2 FragCord;

vec2 cube_intersect(vec3 ro, vec3 rd, float rad, out vec3 outNormal) {
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
    vec3 t0 = ro / rd;
    vec3 t1 = (ro + vec3(rad)) / rd;
    vec3 t2 = min(t0, t1);
    vec3 t3 = max(t0, t1);
    vec2 l = vec2(max(max(t2.x, t2.y), t2.z), min(min(t3.x, t3.y), t3.z));
    if (l.x > l.y || l.y < 0.0) return vec2(-1.0);
    return l;
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

    vec3 norm;
    vec2 ti = cube_intersect(vec3(0,0,1)-ro, rd, 0.1, norm);
    if (ti.x > 0)
        FragColor = vec4(dot(normalize(vec3(0.1, 0.5, -1)), norm));
    else
        FragColor = vec4(0);
}