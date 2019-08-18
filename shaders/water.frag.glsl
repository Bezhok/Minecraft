#version 330 core
in vec4 light_position;
in vec2 tex_coord;
in float normal_id;

out vec4 color;

uniform sampler2D ourTexture;


uniform vec3 sun_dir;
uniform bool is_day;

const vec4 fog_color_day = vec4(0.6, 0.8, 1.0, 1.0);
const vec4 fog_color_night = vec4(0.109f, 0.156f, 0.313f, 1.0);

const vec3 sun_color = vec3(1.0, 1.0, 1.0);

//TODO should be connected with render distance
const float fogdensity = .000011*.000011;

float get_cube_scalar()
{
	float scalar = 0.f; //dot(norm, norm_sun_dir);



	if (normal_id < 1.f) {
		scalar = -sun_dir.x;
	} else if (normal_id < 2.f) {
		scalar = sun_dir.x;
	} else if (normal_id < 3.f) {
		scalar = -sun_dir.y;
	} else if (normal_id < 4.f) {
		scalar = sun_dir.y;
	} else if (normal_id < 5.f) {
		scalar = -sun_dir.z;
	} else if (normal_id < 6.f) {
		scalar = sun_dir.z;
	}

	return scalar;
}

void main()
{
	float scalar = get_cube_scalar();


	float diff = clamp(scalar, 0.2, 0.9);


	vec2 c = light_position.xy*0.5f+0.5f;
	vec4 texColor = texture(ourTexture, tex_coord);


	if (normal_id < 2 || normal_id > 4)
		texColor *= vec4(0.9, 0.9, 0.9, 1);
	else if (normal_id < 4.f) // positive y
		texColor *= vec4(0.95, 0.95, 0.95, 1);
	else if (normal_id < 3.f) // negative y
		texColor *= vec4(0.85, 0.85, 0.85, 1);

	
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float currentDepth = light_position.z*0.5+0.5;




	texColor *= vec4(sun_color*diff, 1);


	if (texColor.a < 0.1)
		discard;

	

	float fog = clamp(exp(-fogdensity * z * z * z*z), 0.2, 1);

	if (is_day) {
		color = mix(fog_color_day, texColor, fog);
	} else {
		color = mix(fog_color_night, texColor, fog);
	}
}
