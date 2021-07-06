#version 400
out vec4 outputColor;
in vec3 texCoord;

uniform samplerCube skyBox_data; 

void main()
{
	outputColor = texture(skyBox_data, texCoord);
}