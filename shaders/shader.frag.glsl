#version 330 core
in vec4 position;
in vec2 tex_coord;

out vec4 color;

uniform sampler2D ourTexture;


void main()
{
	vec4 texColor;
	if (position.w < 1) // if on top
		 texColor = texture(ourTexture, tex_coord)*vec4(0.7, 0.7, 0.7, 1.0);
	else 
		 texColor = texture(ourTexture, tex_coord)*vec4(0.6, 0.6, 0.6, 1.0);

	if (texColor.a < 0.1)
		discard;

	color = texColor;
}
