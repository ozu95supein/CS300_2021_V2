#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

out vec2 outUV;

void main()
{
	mat4 MVP = u_P * u_V * u_M;
	outUV = aUV;
	gl_Position = MVP * aPosition;
}