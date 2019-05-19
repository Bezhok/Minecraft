#version 330 core
in vec4 position;
in vec2 tex_coord;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
	if (position.w < .0) // if on top
		color = texture(ourTexture, tex_coord);
	else 
		color = texture(ourTexture, tex_coord)*vec4(0.65, 0.65, 0.65, 1.0);

	////fog
//	float z = gl_FragCoord.z / gl_FragCoord.w;
//	if (z>10) {
//	float transp = clamp(exp(0.01 * z), 0.2, 1);
//	color *= vec4(0.6, 0.8, 1.0, 1);
//	}

}