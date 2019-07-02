#version 440 core

layout (location = 0) in vec3 vCubeDir;

layout (location = 0) out vec4 fragColor;

uniform samplerCube uSkybox;

void main() {
    fragColor = texture(uSkybox, vCubeDir);
}
