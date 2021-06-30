#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 u_M;

out vec2 outUV;
out int debugBool;

void main()
{
	outUV = aUV;
	gl_Position = u_M * aPosition;
}