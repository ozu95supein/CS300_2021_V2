#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in vec4 aAvgNormal;

out vec2 outUV;
out vec3 CubeMapTexCoord;
out vec3 ReflectTexCoord3;
out vec3 RefractTexCoord3;
out vec4 ResultingNormalWorldspace4;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;
uniform vec4 camera_position;
uniform int usingFaceNormals;

float refraction_index = 1.33;

void main()
{
	mat4 MVP = u_P * u_V * u_M;
	mat4 Q_world = transpose(inverse(u_M));
	
	vec4 posWorldspace = u_M * aPosition;
	vec4 IncidentVectorModelSpace = normalize(posWorldspace - camera_position);
	if(usingFaceNormals == 0)
	{
		ResultingNormalWorldspace4 = normalize(Q_world * aNormal);
		vec4 ReflectModelspace = reflect(IncidentVectorModelSpace, ResultingNormalWorldspace4);
		vec4 RefractModelspace = refract(IncidentVectorModelSpace, ResultingNormalWorldspace4, refraction_index);
		ReflectTexCoord3 = normalize(ReflectModelspace.xyz);
		RefractTexCoord3 = normalize(RefractModelspace.xyz);
	}
	else
	{
		ResultingNormalWorldspace4 = normalize(Q_world * aAvgNormal);
		vec4 ReflectModelspace = reflect(IncidentVectorModelSpace, ResultingNormalWorldspace4);
		vec4 RefractModelspace = refract(IncidentVectorModelSpace, ResultingNormalWorldspace4, refraction_index);
		ReflectTexCoord3 = normalize(ReflectModelspace.xyz);
		RefractTexCoord3 = normalize(RefractModelspace.xyz);
	}

	outUV = aUV;
	CubeMapTexCoord = vec3(aPosition.xyz);
    gl_Position = MVP * aPosition;
}