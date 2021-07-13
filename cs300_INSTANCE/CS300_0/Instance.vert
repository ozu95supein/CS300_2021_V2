#version 400
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 outUV;

uniform mat4 u_P;
uniform mat4 u_V;


void main()
{ 
    gl_Position = u_P * u_V * aInstanceMatrix * aPosition; 
    outUV = aUV;
}