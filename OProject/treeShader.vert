#version 150

// Ultra-minimal: Pass-through of position and nothing more.

in  vec3 inPosition;

void main(void)
{
	gl_Position = projMatrix *camMatrix* mdlMatrix * vec4(inPosition, 1.0);
}
