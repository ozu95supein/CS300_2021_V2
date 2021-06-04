#version 400
out vec4 outputColor;
in vec2 outUV;
in vec4 position_cameraspace;
in vec4 normal_cameraspace;

uniform sampler2D texture_data; // smallColors.png
uniform int texture_toggle;

uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;

void main()
{
	if(texture_toggle == 1)
	{
		vec4 meshColor4 = texture(texture_data, outUV); 
		vec3 meshColor3 = vec3(meshColor4.x, meshColor4.y, meshColor4.z);
		vec3 K_a = meshColor3 * materialAmbient;
		vec3 I_ambient = lightAmbient * K_a;
		



		outputColor = vec4(I_ambient, 1.0f);
	}
	else
	{
		vec3 meshColor3 = vec3(outUV.x, outUV.y, 0);
		vec3 K_a = meshColor3 * materialAmbient;
		vec3 I_ambient = lightAmbient * K_a;




		outputColor = vec4(I_ambient, 1.0f);
	}
}