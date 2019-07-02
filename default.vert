#version 440 core

// Inputs
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 tC;
uniform mat4x4 uProjection;
uniform mat4x4 uView;
uniform mat4x4 uModel;
uniform mat4x4 uRotMat;

// Outputs
layout(location = 0) out vec2 vTexture;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec3 vFragPos;

void main() {
    // Farbe an Fragment-Shader übergeben
    vNormal = mat3(transpose(inverse(uModel))) * aNormals;
    //vNormal = aNormals;

    // Textur-Koordinaten an Fragment-Shader übergebe
    vTexture = tC.st;

    vFragPos = vec3(uModel * vec4(aPosition, 1.0));

    // Vertex-Position übernehmen
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}
