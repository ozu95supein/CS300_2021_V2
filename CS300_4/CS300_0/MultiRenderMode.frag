#version 400
out vec4 outputColor;

in vec2 outUV;
in vec3 CubeMapTexCoord;
in vec4 ResultingNormalWorldspace4;
in vec4 posWorldSpace4;

uniform sampler2D texture_data;		// smallColors.png
uniform samplerCube cubemap_data;	// cubemap

uniform int RenderMode = 0;
uniform vec4 camera_position;
float refractRatio = 1.00f / 1.33f;
void main()
{
	if(RenderMode == 0)			//regular colormap rendering
	{
		outputColor = texture(texture_data, outUV); 
	}
	else if(RenderMode == 1)	//cubemap rendering 1st version
	{
		outputColor = texture(cubemap_data, ResultingNormalWorldspace4.xyz);
	}
	else if(RenderMode == 2)	//cubemap rendering 2nd version
	{
		//outputColor = texture(cubemap_data, CubeMapTexCoord);
		vec4 IncidentVectorModelSpace = normalize(posWorldSpace4 - camera_position);
		vec4 ReflectModelspace = reflect(IncidentVectorModelSpace, normalize(ResultingNormalWorldspace4));
		outputColor = texture(cubemap_data, normalize(ReflectModelspace.xyz));
	}
	else if(RenderMode == 3)	//cubemap rendering Refraction
	{
		//outputColor = texture(cubemap_data, CubeMapTexCoord);
		vec4 IncidentVectorModelSpace = normalize(posWorldSpace4 - camera_position);
		vec4 RefractModelspace = refract(IncidentVectorModelSpace, normalize(ResultingNormalWorldspace4), refractRatio);
		outputColor = texture(cubemap_data, normalize(RefractModelspace.xyz));
	}
}