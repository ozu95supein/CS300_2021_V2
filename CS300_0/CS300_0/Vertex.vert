#version 400

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

out vec2 outUV;
out position_cameraspace;
out normal_cameraspace;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

void main()
{
	mat4 MVP = u_P * u_V * u_M;
	mat4 Q = u_V * u_M;
	Q = inverse(Q);
	Q = transpose(Q);
	position_cameraspace = Q * aPosition;
	normal_cameraspace = Q * aNormal;

	outUV = aUV;

    gl_Position = MVP * aPosition;
}