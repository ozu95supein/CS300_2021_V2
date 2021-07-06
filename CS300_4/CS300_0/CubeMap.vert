#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

out vec3 texCoord;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

void main()
{
	texCoord = vec3(aPosition.xyz);
	gl_Position = u_P * u_V * vec4(aPosition.xyz, 1.0f);
}