#version 330 core
layout (location = 0) in vec4 loc_position;
layout (location = 1) in vec2 loc_tex_coord;


out float normal_id;
out vec4 postition_from_light;
out vec2 tex_coord;


uniform mat4 pvm;
uniform mat4 light_pvm;

void main()
{
	normal_id = loc_position.w;

    gl_Position = pvm * vec4(loc_position.xyz, 1.0f);

	postition_from_light = light_pvm * vec4(loc_position.xyz, 1.0f);

    tex_coord = vec2(loc_tex_coord.x/16.0f, 1-loc_tex_coord.y/16.0f); // TODO should get this data: 16 blocks in x and 16 blocks in y
} 