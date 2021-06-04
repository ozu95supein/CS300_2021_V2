#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

out vec2 outUV;
out vec4 position_cameraspace;
out vec4 normal_cameraspace;
out vec4 lightPosition_cameraspace;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;
uniform vec4 lightPosition;

void main()
{
	mat4 MVP = u_P * u_V * u_M;
	mat4 MV = u_V * u_M;

	mat4 Q = transpose(inverse(MV));

	outUV = aUV;
	position_cameraspace = u_V * u_M * aPosition;
	lightPosition_cameraspace = u_V * u_M * lightPosition;

	normal_cameraspace = Q * aNormal;

    gl_Position = MVP * aPosition;
}