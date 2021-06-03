#version 400
out vec4 outputColor;
in vec2 outUV;

uniform sampler2D texture_data; // smallColors.png
uniform int texture_toggle;

uniform vec3 lightAmbient;
uniform vec3 materialAmbient;
void main()
{
	
	if(texture_toggle == 1)
	{
		outputColor = texture(texture_data, outUV); 
	}
	else
	{
		outputColor = vec4(vec3(outUV.x, outUV.y, 0), 1.0f);
	}
}