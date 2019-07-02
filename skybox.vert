#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 0) out vec3 vCubeDir;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {
    vCubeDir = aPos;
    gl_Position = uProjection * uView * vec4(aPos, 1.0);
}
