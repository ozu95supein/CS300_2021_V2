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
    mat4 ModelView = u_V * aInstanceMatrix;

    //first column
    ModelView[0][0] = 1.0f;
    ModelView[0][1] = 0.0f;
    ModelView[0][2] = 0.0f;

    // Thrid colunm.
    ModelView[2][0] = 0.0; 
    ModelView[2][1] = 0.0; 
    ModelView[2][2] = 1.0; 

    vec4 ResultingPosition = ModelView * aPosition;
    gl_Position = u_P * ResultingPosition;
    //gl_Position = u_P * u_V * aInstanceMatrix * aPosition; 
    outUV = aUV;
}