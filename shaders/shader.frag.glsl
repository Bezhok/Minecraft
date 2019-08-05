#version 330 core
in vec4 position;
in vec2 tex_coord;

out vec4 color;

uniform sampler2D ourTexture;

const vec4 fog_color = vec4(0.6, 0.8, 1.0, 1.0);

//TODO should be connected with render distance
const float fogdensity = .0002*.0002;

void main()
{
	vec4 texColor;
	if (position.w < 1) // if on top
		 texColor = texture(ourTexture, tex_coord)*vec4(0.8, 0.8, 0.8, 1.0);
	else 
		 texColor = texture(ourTexture, tex_coord)*vec4(0.7, 0.7, 0.7, 1.0);

	if (texColor.a < 0.1)
		discard;

	color = texColor;

	
  float z = gl_FragCoord.z / gl_FragCoord.w;
  float fog = clamp(exp(-fogdensity * z * z * z), 0.2, 1);

  color = mix(fog_color, texColor, fog);
}
