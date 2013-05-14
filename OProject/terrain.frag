#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
in vec3 exSurface; // specular
in vec3 outPosition;

in vec3 transformedNormal;

void main(void)
{
	const vec3 light = vec3(1.1, 0.5, 0.0);
	float shade = dot(normalize(transformedNormal), light);
	shade = max(0.0,shade)+0.3;
	vec3 r = reflect(-light, normalize(transformedNormal));
	vec3 v = normalize(-exSurface);
	float specular = dot(r, v);
	if (specular > 0.0)
		specular = 0.2 * pow(specular, 25.0);
	specular = max(specular, 0.0);
	if(outPosition.y<=0.01 && transformedNormal.y >= 0.99) //bara specular på vatten
		shade = shade + specular;

	if(outPosition.y<=0.3 ){
		outColor= ((1-sin(outPosition.y*4))*texture(tex3, texCoord)+sin(outPosition.y*1.5)*texture(tex1, texCoord)) * vec4(shade,shade,shade,1.0);
	}
	else
	{	
		outColor = (sin(transformedNormal.y-0.3)*sin(transformedNormal.y-0.3)*texture(tex1, texCoord)+(1-sin(transformedNormal.y-0.3))*(1-sin(transformedNormal.y-0.3))*texture(tex2, texCoord))*vec4(shade,shade,shade,1.0);
	}
}
