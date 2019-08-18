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


float get_cube_scalar()
{
	float scalar = 0.f; //dot(norm, norm_light_dir);

	if (normal_id < 1.f) {
		scalar = -light_dir.x;
	} else if (normal_id < 2.f) {
		scalar = light_dir.x;
	} else if (normal_id < 3.f) {
		scalar = -light_dir.y;
	} else if (normal_id < 4.f) {
		scalar = light_dir.y;
	} else if (normal_id < 5.f) {
		scalar = -light_dir.z;
	} else if (normal_id < 6.f) {
		scalar = light_dir.z;
	}

	return scalar;
}

void main()
{
	float scalar = get_cube_scalar();
	float diff = clamp(scalar, 0.2, 1.f);

	vec4 texColor = texture(atlas, tex_coord);


	if (normal_id < 2 || normal_id > 4)
		texColor *= vec4(0.9, 0.9, 0.9, 1);
	else if (normal_id < 4.f) // positive y
		texColor *= vec4(0.95, 0.95, 0.95, 1);
	else if (normal_id < 3.f) // negative y
		texColor *= vec4(0.85, 0.85, 0.85, 1);

	
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float currentDepth = postition_from_light.z*0.5+0.5;

	float shadow = 0.0;
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

				shadow += currentDepth - bias > pcf_depth ? 0.2 : 1.0;
			}
		}
		shadow /= 25;
	}


	texColor *= vec4(light_color*diff*shadow, 1);

	if (texColor.a < 0.1)
		discard;

	float fog = clamp(exp(-fog_density * z * z * z), 0.2, 1);
	color = mix(vec4(fog_color, 1.f), texColor, fog);
}
