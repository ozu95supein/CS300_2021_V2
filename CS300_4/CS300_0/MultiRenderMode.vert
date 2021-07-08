#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUV;

out vec2 outUV;
out vec3 CubeMapTexCoord;
out vec3 ReflectTexCoord3;
out vec4 SurfaceNormalWorldspace4;
out vec3 RefractTexCoord;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;
uniform vec4 camera_position;
float refraction_index = 1.33;

void main()
{
	mat4 MVP = u_P * u_V * u_M;
	mat4 Q_world = transpose(inverse(u_M));
	SurfaceNormalWorldspace4 = Q_world * aNormal;

	vec4 posWorldspace = u_M * aPosition;
	vec4 IncidentVectorModelSpace = normalize(posWorldspace - camera_position);
	vec4 normalWorldspace = normalize(u_M * aNormal);
	vec4 ReflectModelspace = reflect(IncidentVectorModelSpace, normalWorldspace);
	vec4 RefractModelspace = refract(IncidentVectorModelSpace, normalWorldspace, refraction_index);
	//pass to frag shader
	ReflectTexCoord3 = normalize(ReflectModelspace.xyz);
	RefractTexCoord = normalize(RefractModelspace.xyz);
	SurfaceNormalWorldspace4 = normalWorldspace;

	outUV = aUV;
	CubeMapTexCoord = vec3(aPosition.xyz);
    gl_Position = MVP * aPosition;
}