#version 330 core
layout (location = 0) in vec4 loc_position;
layout (location = 1) in vec2 loc_tex_coord;

out vec4 position;
out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	position = loc_position;

	position /= 15;

    gl_Position = projection * view * model * vec4(position.xyz, 1.0f);
    tex_coord = vec2(loc_tex_coord.x/16.0f, 1-loc_tex_coord.y/16.0f); // 16 blocks in x and blocks in y
} 