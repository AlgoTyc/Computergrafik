#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec3 aPosition;

layout(location=3) uniform mat4 ModelViewProjectionMatrix;


void main(void)
{
    gl_Position =ModelViewProjectionMatrix * vec4(aPosition,1.0);

}
