#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in vec4 aAvgNormal;
layout(location = 4) in vec4 aTangent;
layout(location = 5) in vec4 aBiTangent;
layout(location = 6) in vec4 aAvgTangent;
layout(location = 7) in vec4 aAvgBiTangent;


out vec2 outUV;
out vec4 position_cameraspace;
out vec4 lightPosition_cameraspace;

out vec4 normal_cameraspace;
out vec4 Avg_normal_camerapsace;
out vec4 Tangent_Cameraspace;
out vec4 BiTangent_Cameraspace;
out vec4 Avg_Tangent_Cameraspace;
out vec4 Avg_BiTangent_Cameraspace;

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
	lightPosition_cameraspace = u_V * lightPosition;

	normal_cameraspace = Q * aNormal;
	
    gl_Position = MVP * aPosition;
	//Tangent and Bitangent Calculations

}