#version 150

out vec4 outColor;
//in vec3 ex_Color;
in vec2 outTexCoord;


//in vec3 transformedNormal;

//uniform mat4 colorMat;


uniform sampler2D tex1;
//uniform mat4 projectionMatrix;

in vec3 outPosition;

void main(void)
{
  //	const vec3 light = vec3(0.4, 0.4, 0.4);
	//float shade = dot(normalize(transformedNormal), light);
	//float a = sin(outTexCoord.s*30)/2+0.5;
	//float b = sin(outTexCoord.t*30)/2+0.5;
	//outColor = vec4(1.0,1.0,1.0,1.0);

	//float a = outTexCoord.s;
	//float b = outTexCoord.t;
	//out_Color=colorMat*vec4(shade ,shade,shade,1.0);

	outColor = texture(tex1, outTexCoord);
}
