#version 330 core
layout (location = 0) in vec4 loc_position;
layout (location = 1) in vec2 loc_tex_coord;


out float normal_id;
out vec4 light_position;
out vec2 tex_coord;
out vec3 normal;
out vec3 frag_pos;

uniform mat4 pvm;
uniform mat4 light_pvm;
uniform float dtime;
uniform mat4 model;

vec3 calc_normal(vec3 pos, vec3 neighbour1, vec3 neighbour2);
float clip_var(float v);
vec3 get_loc_position(vec3 pos);

void main()
{
	normal_id = loc_position.w;

	vec3 pos = get_loc_position(loc_position.xyz);
	vec4 new_loc_position = vec4(pos, 1.f);

	vec3 neighbour1 = loc_position.xyz;
	vec3 neighbour2 = loc_position.xyz;

	int normal_id_int = int(normal_id);

	if (normal_id_int == 11) {
		neighbour1 += vec3(0, 0, 1);
		neighbour2 += vec3(1, 0, 0);
	} else if (normal_id_int == 12) {
		neighbour1 += vec3(0, 0, -1);
		neighbour2 += vec3(1, 0, -1);
	} else if (normal_id_int == 13) {
		neighbour1 += vec3(-1, 0, 0);
		neighbour2 += vec3(-1, 0, 1);
	} else if (normal_id_int == 14) {
		neighbour1 += vec3(-1, 0, 1);
		neighbour2 += vec3(0, 0, 1);
	} else if (normal_id_int == 15) {
		neighbour1 += vec3(1, 0, -1);
		neighbour2 += vec3(1, 0, 0);
	} else if (normal_id_int == 16) {
		neighbour1 += vec3(-1, 0, 0);
		neighbour2 += vec3(0, 0, -1);
	}

	neighbour1 = get_loc_position(neighbour1);
	neighbour2 = get_loc_position(neighbour2);

	normal = calc_normal(pos, neighbour1, neighbour2);

    gl_Position = pvm * new_loc_position;

	frag_pos = vec3(model * new_loc_position);

	light_position = light_pvm * new_loc_position;

    tex_coord = vec2(loc_tex_coord.x/16.0f, 1-loc_tex_coord.y/16.0f); // TODO should get this data: 16 blocks in x and 16 blocks in y
}

vec3 calc_normal(vec3 pos, vec3 neighbour1, vec3 neighbour2)
{
	vec3 vecf = neighbour1-pos;
	vec3 vecs = neighbour2-pos;

	vec3 res = normalize(cross(normalize(vecf), normalize(vecs)));
	return abs(res);
}

float clip_var(float v)
{
	if (v < 0) {
		v = 16 + v;
	} else if (v >= 16) {
		v = v - 16;
	}
	return v;
}

vec3 get_loc_position(vec3 pos)
{
	vec3 position = pos;
	float x = clip_var(position.x);
	float z = clip_var(position.z);

	position.y += sin(dtime*(abs(x-8)/8))/4.f;
	position.y -= cos(dtime*(abs(z-8)/8))/3.5f;

	return position;
}
