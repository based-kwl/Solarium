// ##### CONSTANTS #####
#define pi 3.14159265
#define twopi pi*2.0

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