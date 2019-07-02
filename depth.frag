#version 440 core

// Inputs
layout(location = 0) in vec2 vTex;

// Outputs
layout(location = 0) out vec4 fragColor;

// Uniform-Variablen
uniform sampler2D uTex;
uniform sampler2D uTex2;

void main() {
    // Output-Farbe und Alpha-Wert setzen und auf Output schreiben
    //fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);

    // Farbtextur + Tiefentextur laden und vermischen
    fragColor = texture(uTex, vTex);
    //vec4 fragDepth = texture(uTex2, vTex);
    //fragColor = vec4(fragColor.r, fragColor.r, fragColor.r, .5) + vec4(vec3(fragDepth), .5);
}
