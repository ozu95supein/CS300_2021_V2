#version 400
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 outUV;

uniform mat4 u_P;
uniform mat4 u_V;
uniform vec3 u_camPos;
uniform mat4 u_Scale;

void main()
{ 
    //get vector from object to camera and floor it to the xz plane
    vec3 ObjToCam = u_camPos - aInstanceMatrix[3].xyz;
    ObjToCam.y = 0.0f;
    ObjToCam = normalize(ObjToCam);
    vec3 Normal3 = aNormal.xyz;
    Normal3 = normalize(Normal3);
    //calculate angle between quad normal and ObjToCam using trig
    //length of ObjToCam and Normal are 1 already
    float cosine_alpha = dot(Normal3, ObjToCam);
    float alpha_rad;
    if(ObjToCam.x < 0.0f)
    {
        alpha_rad = - acos(cosine_alpha);
        
    }
    else
    {
        alpha_rad = acos(cosine_alpha);
    }
    //make hard codded rotation matrix along y axis with alpha_rad
    mat4 Rot_Y;
    Rot_Y[0] = vec4(cos(alpha_rad), 0.0f, -sin(alpha_rad), 0.0f);
    Rot_Y[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    Rot_Y[2] = vec4(sin(alpha_rad), 0.0f, cos(alpha_rad), 0.0f);
    Rot_Y[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    mat4 Model = aInstanceMatrix *Rot_Y * u_Scale;
    mat4 MVP = u_P * u_V * Model;
    gl_Position = MVP * aPosition; 
    outUV = aUV;
}