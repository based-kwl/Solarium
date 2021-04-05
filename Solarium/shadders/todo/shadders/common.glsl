// ##### CONSTANTS #####
// http://www.mimirgames.com/articles/programming/digits-of-pi-needed-for-floating-point-numbers/
// Full-Precision (32-Bit) Floating-Point Numbers can't represent Pi any further than ~3.14159265 but it's
// good to the compiler give a few extra digits so it can choose the best Floating-Point Value possible
#define pi 3.1415926535897932384626433832795028841971693993751058209749445923078164
#define twopi pi*2.0

// ##### NOISE #####
// Cyclic Noise (2D)
float cyclic(vec2 coord){
    float component1 = cos(coord.x);
    float component2 = cos(coord.y);
    return abs(component1+component2)/2.0;
}

// Fractal Brownian Motion (FBM) Cyclic Noise (2D)
float fbmcyclic(vec2 coord, uint octaves){
    float value = 0.0;
    float scale = 1.0;
    float atten = 0.5;
    for(uint i = 0U; i < octaves; i++){
        value += cyclic(coord*scale)*atten;
        scale *= 2.0;
        atten *= 0.5;
    }
    return value;
}

// Cyclic Noise
float cyclic(vec3 coord){
    float component1 = cos(coord.x);
    float component2 = cos(coord.y);
    float component3 = cos(coord.z);
    return abs(component1+component2+component3)/3.0;
}

// Fractal Brownian Motion (FBM) Cyclic Noise
float fbmcyclic(vec3 coord, uint octaves){
    float value = 0.0;
    float scale = 1.0;
    float atten = 0.5;
    for(uint i = 0U; i < octaves; i++){
        value += cyclic(coord*scale)*atten;
        scale *= 2.0;
        atten *= 0.5;
    }
    return value;
}

// Dave_Hoskins Hash33: https://www.shadertoy.com/view/4djSRW
vec3 hash(vec3 p3){
    p3 = fract(p3*vec3(0.1031, 0.1030, 0.0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy+p3.yxx)*p3.zyx);
}

// Worley Noise
float worley(vec3 coord){
    vec3 cell = floor(coord);
    float mindist = 1000.0;
    for(int z = -1; z < 2; z++){
    for(int y = -1; y < 2; y++){
    for(int x = -1; x < 2; x++){
        vec3 ncell = cell+vec3(x, y, z);
        vec3 point = ncell+hash(ncell);
        mindist = min(dot(coord-point, coord-point), mindist);
    }
    }
    }
    return sqrt(mindist);
}

// Fractal Brownian Motion (FBM) Worley Noise
float fbmcyclic(vec3 coord, uint octaves){
    float value = 0.0;
    float scale = 1.0;
    float atten = 0.5;
    for(uint i = 0U; i < octaves; i++){
        value += worley(coord*scale)*atten;
        scale *= 2.0;
        atten *= 0.5;
    }
    return value;
}

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