#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;
out vec2 texCoord;
out vec3 transformedNormal;
out vec3 outPosition;
out vec3 exSurface;


uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform mat4 mdlMatrix;

void main(void)
{
	outPosition = inPosition;
	mat3 normalMatrix1 = mat3(mdlMatrix);
	transformedNormal = normalMatrix1 * inNormal;
	texCoord = inTexCoord;
	gl_Position = projMatrix *camMatrix* mdlMatrix * vec4(inPosition, 1.0);
	exSurface =  vec3(mdlMatrix * vec4(inPosition, 1.0));
}
