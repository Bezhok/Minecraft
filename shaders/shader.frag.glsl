#version 330 core
in vec4 position;
in vec2 tex_coord;
in vec3 normal;
in vec3 frag_pos;

out vec4 color;

uniform sampler2D ourTexture;
uniform vec3 sun_dir;
uniform bool is_day;

const vec4 fog_color_day = vec4(0.6, 0.8, 1.0, 1.0);
const vec4 fog_color_night = vec4(0.109f, 0.156f, 0.313f, 1.0);

const vec3 sun_color = vec3(1.0, 1.0, 1.0);

//TODO should be connected with render distance
const float fogdensity = .000011*.000011;

void main()
{
	vec3 norm = normalize(normal);
//	vec3 light_dir = normalize(light_pos - frag_pos);
	vec3 norm_sun_dir = normalize(sun_dir);

	float diff;

	if (is_day) {
		diff = clamp(dot(norm, norm_sun_dir), 0.3, 0.9);
	} else {
		diff = max(dot(norm, norm_sun_dir)/8.f, 0.05);
	}

	vec4 texColor;// = texture(ourTexture, tex_coord);
	if (position.w < 1) // positive y
		texColor = texture(ourTexture, tex_coord)*vec4(0.8, 0.8, 0.8, 1);
	else if (position.w < 2) // negative y
		texColor = texture(ourTexture, tex_coord)*vec4(0.5, 0.5, 0.5, 1);
	else 
		texColor = texture(ourTexture, tex_coord)*vec4(0.7, 0.7, 0.7, 1);

	
	texColor *= vec4(sun_color*diff, 1);



	if (texColor.a < 0.1)
		discard;

//	color = texColor;

	
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fog = clamp(exp(-fogdensity * z * z * z*z), 0.2, 1);

	if (is_day) {
		color = mix(fog_color_day, texColor, fog);
	} else {
		color = mix(fog_color_night, texColor, fog);
	}
}
