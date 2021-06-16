#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec2 vTexCoord;
layout (location = 1) out vec3 vNormal;
layout (location = 2) out vec3 vFragPos;

layout (location = 3) uniform mat4 uProjectionMatrix;
layout (location = 4) uniform mat4 uViewMatrix;
layout (location = 5) uniform mat4 uModelMatrix;
layout (location = 20) uniform mat3 uNormalMatrix;

void main(void)
{
    vec4 vertHom = vec4(aPosition, 1.0);
    vTexCoord = aTexCoord.st;

    // Normalize and pass on normals
    vNormal =normalize(uNormalMatrix*aNormal);

    // Calculate and normalize fragment position
    vec4 fragInWorld= uModelMatrix*vec4(aPosition,1.0);
    vFragPos = normalize(fragInWorld.xyz);

    gl_Position = uProjectionMatrix*uViewMatrix*uModelMatrix*vertHom;
}
