#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable
#define NUM_LS 5

layout(location = 0)  out vec4 fragColor;

layout(location = 0)  in vec2 vTexCoord;
layout(location = 1)  in vec3 vNormal;
layout(location = 2)  in vec3 vFragPos;

layout(location = 6)  uniform vec4 ringFarbe;
layout(location = 7)  uniform sampler2D tex0;
// Phong Parameters
layout (location = 8)  uniform vec3 uViewPosition;
layout (location = 9)  uniform vec3 uLightPosition;
layout (location = 10) uniform float uKa; // Ambient
layout (location = 11) uniform float uKd; // Diffuse
layout (location = 12) uniform float uKs; // Specular
layout (location = 13) uniform float uShininess;


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
//location 13,14,15,16
layout (location = 14) uniform Material material;

struct PointLight { // -base alignment  -aligned offset -total bytes

    vec3 position;  // 16                0                   16

    vec3 color;     // 16                16                  32

    float ambient;  // 4                 32                  36
    float diffuse;  // 4                 36                  40
    float specular; // 4                 40                  44

    float constant; // 4                 44                  48
    float linear;   // 4                 48                  52
    float quadratic;// 4                 52                  56
};

layout (std140) uniform lightBlock { // Create a UBO with layout std140 and Title lightBlock
        PointLight lights[NUM_LS];
};

layout (location = 19) uniform samplerCube cubeMap;

vec3 calcPhongLight(PointLight light,vec3 viewDirection, vec3 lightDirection)
{


    // Calculate Phong-Illumination here
    vec3 ambient=material.ambient*vec3(light.ambient);

    float dist=distance(vFragPos,light.position);
    float fAtt=(1.0/(light.constant +light.linear*dist+light.quadratic*dist*dist));

    vec3 diffuse=material.diffuse*vec3(fAtt*light.diffuse*max(0.0,dot(vNormal,lightDirection)));

    vec3 reflectLight=reflect(-lightDirection,vNormal);
    float phongExponent=pow(max(0.0,dot(viewDirection,reflectLight)),material.shininess*128);
    vec3 specular=material.specular*vec3(fAtt*light.specular*phongExponent);

    return (ambient + diffuse + specular)*light.color;
}

void main(void) {
    //..
    // Calculate viewDirection & lightDirection -> normalise
    // Calculate Phong-Illumination: vec3 result
    vec3 viewDirection=normalize( uViewPosition-vFragPos);
    //vec3 reflectViewDirect=reflect(-viewDirection,vNormal);
     vec3 refractViewDirect=refract(viewDirection,vNormal,1.0/1.52);
    vec3 result = vec3(0.0);
    for ( int i = 0; i <NUM_LS; i++)
    {
       vec3 lightDirection=normalize(uLightPosition-vFragPos);
       result += calcPhongLight(lights[i], viewDirection, lightDirection);
    }


    vec4 texel = texture(cubeMap, refractViewDirect);
    fragColor = vec4(texel.rgb*result,1.0);
}



