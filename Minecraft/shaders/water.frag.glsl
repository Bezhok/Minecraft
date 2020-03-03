#version 330 core
in vec4 postition_from_light;
in vec2 tex_coord;
in float normal_id;
in vec3 normal;
in vec3 frag_pos;

out vec4 color;

uniform sampler2D atlas;
uniform vec3 light_dir;
uniform vec3 light_color;
uniform vec3 fog_color;
uniform float fog_density;
uniform vec3 view_pos;

float specular_strength = 0.5f;

float calc_scalar();
float calc_fog_saturation();
float calc_specular();
float calc_diffuse();

void main()
{
	vec3 texColor = vec3(texture(atlas, tex_coord));
	texColor *= light_color*(calc_diffuse()+calc_specular());

	texColor = mix(texColor, fog_color, calc_fog_saturation());
	color = vec4(texColor, 0.9);
}

float calc_fog_saturation()
{
	float z = gl_FragCoord.z / gl_FragCoord.w;
	return clamp(1-exp(-fog_density * z * z * z), 0, 0.8);
}

float calc_specular()
{
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = min(pow(max(dot(view_dir, reflect_dir), 0.0), 4), 4);

	return specular_strength * spec;
}

float calc_diffuse()
{
	float scalar = 0;
	int normal_id_int = int(normal_id);
	if (normal_id_int < 10) { //if it's not positive y
		scalar = calc_scalar();
	} else {
		scalar = dot(normal, light_dir);
	}

	return clamp(scalar, 0.1, 1.f);
}

float calc_scalar()
{
	float scalar = 0.f; //dot(norm, norm_light_dir);

	int normal_id_int = int(normal_id);
	if (normal_id_int == 1) {
		scalar = -light_dir.x;
	} else if (normal_id_int == 2) {
		scalar = light_dir.x;
	} else if (normal_id_int == 3) {
		scalar = -light_dir.y;
	} else if (normal_id_int == 4) {
		scalar = light_dir.y;
	} else if (normal_id_int == 5) {
		scalar = -light_dir.z;
	} else if (normal_id_int == 6) {
		scalar = light_dir.z;
	}

	return scalar;
}