#version 440 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable


// Ignore the following line, it 's not relevant for now

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(location = 0, rgba8) readonly restrict uniform image2D inImage;
layout(location = 1, rgba8) writeonly restrict uniform image2D outImage;
uniform bool hBlur;
uniform int gLevel;
uniform float gVec[15];

vec4 gaussBlur(ivec2 pos) {
    // N = 5: 0.0359303, 0.238323, 0.451494, 0.238323, 0.0359303

    vec4 res = vec4(0);

    if(hBlur) {
        for(int i = -gLevel/2; i < gLevel/2; i++){
            res += gVec[i + gLevel/2] * imageLoad(inImage, ivec2(pos.x + i, pos.y));
        }
    } else {
        for(int i = -gLevel/2; i < gLevel/2; i++){
            res += gVec[i + gLevel/2] * imageLoad(inImage, ivec2(pos.x, pos.y + i));
        }
    }
    return res;
}

vec4 grayscale(vec4 texIn) {
    texIn.rgb = vec3((texIn.r + texIn.g + texIn.b)/3);
    return texIn;
}

vec4 colorFilter(vec4 texIn, int type) {
    switch(type) {
        case 1:
            texIn.r = 0;
            break;
        case 2:
            texIn.g = 0;
            break;
        case 3:
            texIn.b = 0;
            break;
        default:
            break;
    }
    return texIn;
}

void main() {
    ivec2 position = ivec2(gl_GlobalInvocationID.xy);
    vec4 texel = imageLoad(inImage, position);

    /*workImage = inImage;

    float gaussX[5] = {0.0359303, 0.238323, 0.451494, 0.238323, 0.0359303};
    // float divider = 1.0 / (0.0359303 + 0.238323 + 0.451494 + 0.238323 + 0.0359303);

    for(int x = 2; x < gl_GlobalInvocationID.x; x+=5){
        workImage[x - 2] *= gaussX[0];
        workImage[x - 1] *= gaussX[1];
        workImage[x] *= gaussX[2];
        workImage[x + 1] *= gaussX[3];
        workImage[x + 2] *= gaussX[4];
    }*/

    // texel.rgb = vec3((texel.r + texel.g + texel.b)/3);
    // texel = grayscale(texel);
    // texel = colorFilter(texel, 1);
    texel = gaussBlur(position);
    imageStore(outImage, position, texel);
}
