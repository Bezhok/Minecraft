#version 330 core
layout (location = 0) in vec4 loc_position;
layout (location = 1) in vec2 loc_tex_coord;

out vec4 position;
out vec2 tex_coord;

uniform mat4 model;
uniform mat4 pvm;

void main()
{
	position = loc_position;

    gl_Position = pvm * vec4(position.xyz, 1.0f);
    tex_coord = vec2(loc_tex_coord.x/16.0f, 1-loc_tex_coord.y/16.0f); // TODO should get this data: 16 blocks in x and 16 blocks in y
} 