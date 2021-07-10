#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in vec4 aAvgNormal;

out vec2 outUV;
out vec3 CubeMapTexCoord;
out vec4 ResultingNormalWorldspace4;
out vec4 posWorldSpace4;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;
uniform int usingFaceNormals;

void main()
{
	mat4 MVP = u_P * u_V * u_M;
	mat4 Q_world = transpose(inverse(u_M));
	
	posWorldSpace4 = u_M * aPosition;
	if(usingFaceNormals == 0)
	{
		ResultingNormalWorldspace4 = normalize(Q_world * aNormal);	
	}
	else
	{
		ResultingNormalWorldspace4 = normalize(Q_world * aAvgNormal);
	}

	outUV = aUV;
	CubeMapTexCoord = vec3(aPosition.xyz);
    gl_Position = MVP * aPosition;
}