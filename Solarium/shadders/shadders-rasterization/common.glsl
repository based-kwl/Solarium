/*// ##### RNG #####
// Random Number Generation made by Michael0884
// https://www.shadertoy.com/view/wltcRS
uint ns;
#define INIT_RNG ns = 185730U*uint(frame)+uint(fragCoord.x+fragCoord.y*resolution.x);

void pcg(){
    uint state = ns*747796405U+2891336453U;
    uint word = ((state >> ((state >> 28U) + 4U)) ^ state) * 277803737U;
    ns = (word >> 22U) ^ word;
}

float rand(){pcg(); return float(ns)/float(0xffffffffU);}
vec2 rand2(){return vec2(rand(), rand());}
vec3 rand3(){return vec3(rand(), rand(), rand());}
vec4 rand4(){return vec4(rand(), rand(), rand(), rand());}

// Normalized Random
// https://www.shadertoy.com/view/WttyWX
vec2 nrand2(float sigma, vec2 mean){vec2 Z = rand2(); return mean+sigma*sqrt(-2.0*log(Z.x   ))*vec2(cos(twopi*Z.y), sin(twopi*Z.y));}
vec3 nrand3(float sigma, vec3 mean){vec4 Z = rand4(); return mean+sigma*sqrt(-2.0*log(Z.xxy ))*vec3(cos(twopi*Z.z), sin(twopi*Z.z), cos(twopi*Z.w));}
vec4 nrand4(float sigma, vec4 mean){vec4 Z = rand4(); return mean+sigma*sqrt(-2.0*log(Z.xxyy))*vec4(cos(twopi*Z.z), sin(twopi*Z.z), cos(twopi*Z.w), sin(twopi*Z.w));}*/