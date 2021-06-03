#version 400

out vec4 outputColor;
in vec2 outUV;
in vec4 position_cameraspace;
in vec4 normal_cameraspace;

uniform sampler2D texture_data; 
uniform int texture_toggle;

//uniform parameters for 1 manual lightsource and material
//lighting
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 lightAmbient;

//material
uniform vec3  materialAmbient;

void main()
{
	if(texture_toggle == 1)
	{
		vec3 meshColor = texture(texture_data, outUV); 
		vec3 K_a = meshColor * materialAmbient;
		vec3 I_ambient = K_a;
		outputColor = vec4(I_ambient, 1.0f);
	}
	else
	{
		vec3 meshColor = vec4(vec3(outUV.x, outUV.y, 0), 1.0f); 
		vec3 K_a = meshColor * materialAmbient;
		vec3 I_ambient = K_a;
		outputColor = vec4(I_ambient, 1.0f);
	}
}