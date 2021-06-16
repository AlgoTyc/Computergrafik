
#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

// output written to the screen is always location 0
layout(location = 0) out vec4 fragColor;
layout(location = 7) uniform sampler2D tex0;
                     in vec2 outTexCoord;
layout(location = 8) uniform float slider;


void main() {

    // Read from  texture
    fragColor = texture2D(tex0, vec2(slider+outTexCoord.x, outTexCoord.y));
}
