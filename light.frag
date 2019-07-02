#version 440 core

// Inputs
// layout(location = 0) in vec3 vColor;

uniform vec3 uLightColor;

// Outputs
layout(location = 0) out vec4 fragColor;

void main() {
    // Output-Farbe und Alpha-Wert setzen und auf Output schreiben
    fragColor = vec4(uLightColor, 1.0);
}
