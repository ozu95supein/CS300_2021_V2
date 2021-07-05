#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

out vec3 texCoord;
out vec4 position_cameraspace;
out vec4 normal_cameraspace;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

void main()
{
	texCoord = vec3(aPosition.xyz);
	mat4 MVP = u_P * u_V * u_M;
	
	position_cameraspace = u_V * u_M * aPosition;
    gl_Position = MVP * aPosition;
}