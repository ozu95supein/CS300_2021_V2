#version 400
out vec4 outputColor;
in vec3 texCoord;
in vec4 position_cameraspace;
in vec4 normal_cameraspace;

uniform samplerCube skyBox; 
uniform int texture_toggle = 1;

void main()
{
	
}