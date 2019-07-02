#version 440 core
#extension GL_ARB_explicit_uniform_location : enable
#define NUM_LS 5

/*
// Inputs
layout(location = 0) in vec2 texCoordinates;

// Outputs
layout(location = 0) out vec4 fragColor;

// Uniform-Variablen
uniform float uMove = 0;
uniform sampler2D tex0;

void main() {
    // Output-Farbe aus Textur setzen und Move-Variable einrechnen und auf Output schreiben
    fragColor = texture(tex0, texCoordinates + uMove);
}
*/


//..
layout(location = 0) in vec2 vTexture;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vFragPos;

layout(location = 0) out vec4 fragColor;

// Texture + Move
uniform float uMove = 0;
uniform sampler2D tex0;
uniform samplerCube cubeMap;

// Material
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Phong Parameters
uniform vec3 uViewPosition;
uniform vec3 uLightPosition;
uniform float uKa; // Ambient
uniform float uKd; // Diffuse
uniform float uKs; // Specular
uniform float uShininess = 1;
uniform float uConstant = 1.0;
uniform float uLinear = 0.114;
uniform float uQuadratic = 0.0007;
layout(location = 100) uniform Material material;

struct PointLight {     // base alignment //    aligned offset //   total bytes
    vec3 position;      // 16                   0                   16
    vec3 color;         // 16                   16                  32
    float ambient;      // 4                    32                  36
    float diffuse;      // 4                    36                  40
    float specular;     // 4                    40                  44
    float constant;     // 4                    44                  48
    float linear;       // 4                    48                  52
    float quadratic;    // 4                    52                  56
};

layout (std140) uniform lightBlock { // Create a UBO with layout std140 and Title lightBlock
    PointLight lights[NUM_LS];
};

vec3 calcPhongLight(PointLight light, vec3 viewDirection, vec3 lightDirection) {
    // Vars
    vec3 normals = normalize(vNormal);
    vec3 reflectDirection = reflect(-lightDirection, normals);

    // Ambient Licht
    vec3 ambientAnteil = vec3(uKa);
    vec3 ambient = light.ambient + material.ambient;

    // Diffuse Licht
    vec3 diffuseAnteil = vec3(uKd);
    float diff = max(dot(normals, lightDirection), 0.0);
    vec3 diffuse = (light.diffuse + material.diffuse) * diff;

    // Specular Licht
    vec3 specularAnteil = vec3(uKs);
    float spec = max(dot(viewDirection, reflectDirection), 0.0);
    vec3 specular = (light.specular + material.specular) * pow(spec, material.shininess * 128);

    // Attenuation
    float dist = distance(vFragPos, light.position);
    float fatt = 1.0/(light.constant + light.linear * dist + light.quadratic * pow(dist, 2));
    return (ambient + fatt * diffuse + fatt * specular) * light.color;
}

void main() {

    // Cubemap Reflektion
    vec4 texe1 = texture(cubeMap, refract(reflect(vNormal, normalize(uViewPosition - vFragPos)), vNormal, 1.0/2.42));
    //vec4 texel = texture(tex0, vTexture + uMove);
    //vec3 objectColor = texel.rgb;
    //vec3 objectColor = vec3(190/255.0f,154/255.0f,100/255.0f);
    vec3 result = vec3(0.0);
    for (int i = 0; i < NUM_LS; i++) {
        //if(i != 2){
            vec3 lightDir =  normalize(lights[i].position - vFragPos);
            vec3 viewDir = normalize(uViewPosition - vFragPos);
            result += calcPhongLight(lights[i], viewDir, lightDir);
        //}
    }
    fragColor = vec4(texe1 * vec4(result, 1.0f));
    //fragColor = vec4(result, 1.0);

}

/*float betrag(vec3 vInput){
    return sqrt(pow(vInput[0],2) + pow(vInput[1],2) + pow(vInput[2],2));
}

vec3 calcPhongLight(vec3 viewDirection, vec3 lightDirection)
{
    float iAmbient = uKa;
    float NtoL = dot(vNormal, lightDirection);
    float NtoLAbs = betrag(vNormal) * betrag(lightDirection);
    float cosPhi = NtoL/NtoLAbs;
    float iDiffus = uKd * cosPhi;
    if(cosPhi < 0.0) {
        iDiffus = 0.0;
    }
    vec3 vR = reflect(lightDirection, vNormal);
    float VtoR = dot(viewDirection,vR);
    float VtoRAbs = betrag(viewDirection) * betrag(vR);
    float cosAlpha = VtoR/VtoRAbs;
    float iSpecular = uKs * cosAlpha;
    if(cosAlpha < 0.0) {
        iSpecular = 0.0;
    }
    // Calculate Phong-Illumination here
    return vec3(iAmbient + iDiffus + iSpecular);
}

void main() {
    //..
    // Calculate viewDirection & lightDirection -> normalise
    // Calculate Phong-Illumination: vec3 result
    //vec4 texel = texture(<texture>, vTexCoord);
    //fragColor = vec4((texel.rgb * result), 1.0);
    fragColor = texture(tex0, texCoordinates + uMove);
}*/

