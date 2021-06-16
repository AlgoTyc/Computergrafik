#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location=1) uniform vec3 sunColor;
layout(location= 0)  out vec4 fragColor;

void main(void)
{
    gl_FragColor = vec4(sunColor,1.0);
}
