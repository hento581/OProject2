#version 150

out vec4 outColor;

in vec2 outTexCoord;

uniform sampler2D tex1;


in vec3 outPosition;

void main(void)
{

	outColor = texture(tex1, outTexCoord);
}
