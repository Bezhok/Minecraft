#version 330 core
layout (location = 0) in vec3 loc_position;

uniform mat4 pvm;

void main()
{
    gl_Position = pvm * vec4(loc_position.xyz, 1.0f);
} 