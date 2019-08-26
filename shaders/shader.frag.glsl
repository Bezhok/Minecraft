#version 330 core
in vec4 postition_from_light;
in vec2 tex_coord;
in float normal_id;

out vec4 color;

uniform sampler2D atlas;
uniform sampler2D shadow_map;
uniform vec3 light_dir;
uniform vec3 light_color;
uniform vec3 fog_color;
uniform float fog_density;


float calc_scalar();
float calc_shadow_saturation();
float calc_fog_saturation();
float calc_diffuse();

void main()
{

	color = texture(atlas, tex_coord);
	float alpha = color.a;
	vec3 texColor = color.xyz;

	float shadow = calc_shadow_saturation();
	float diffuse = calc_diffuse();
	texColor *= light_color*diffuse*(1-shadow);

	if (alpha < 0.1)
		discard;

	texColor = mix(texColor, fog_color, calc_fog_saturation());
	color = vec4(texColor, alpha);
}

float calc_fog_saturation()
{
	float z = gl_FragCoord.z / gl_FragCoord.w;
	return clamp(1-exp(-fog_density * z * z * z), 0, 0.8);
}

float calc_diffuse()
{
	return clamp(calc_scalar(), 0.3, 0.9f);
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

float calc_shadow_saturation()
{
	float shadow = 0.0;

	float currentDepth = postition_from_light.z*0.5+0.5;
	float bias = 0.0009;
	vec2 c = postition_from_light.xy*0.5f+0.5f;
	if (currentDepth > 1) {
		shadow = 1;
	} else {
		vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
		for(int x = -2; x <= 2; ++x)
		{
			for(int y = -2; y <= 2; ++y)
			{
				float pcf_depth = texture(shadow_map, c + vec2(x, y) * texel_size).r;

				shadow += currentDepth - bias > pcf_depth ? 0.7 : 0;
			}
		}
		shadow /= 25;
	}

	return shadow;
}