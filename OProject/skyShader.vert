#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;
//out vec3 ex_Color;
out vec2 outTexCoord;
out vec3 outPosition;



//out vec3 transformedNormal;

//uniform mat4 colorMat;
uniform mat4 mdlMatrix;

uniform mat4 camMatrix;

uniform mat4 projMatrix;


//uniform sampler2D texUnit;

void main(void)
{
	
	
	//mat3 normalMatrix1 = mat3(mdlMatrix);
	
	//transformedNormal = normalMatrix1 * in_Normal;

	outPosition = inPosition;
	
	outTexCoord = inTexCoord;

	gl_Position = projMatrix*camMatrix*mdlMatrix*vec4(inPosition, 1.0);
}
