#version 150

// Set-to-white

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex1;

void main(void)
{
	
	outColor = texture(tex1, texCoord);


}
