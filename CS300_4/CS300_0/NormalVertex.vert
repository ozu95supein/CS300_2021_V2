#version 400
layout(location = 0) in vec4 aPosition;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

void main()
{
	mat4 MVP = u_P * u_V * u_M;

    gl_Position = MVP * aPosition;
}