#version 400
out vec4 outputColor;
in vec2 outUV;
in vec3 CubeMapTexCoord;

uniform sampler2D texture_data;		// smallColors.png
uniform samplerCube cubemap_data;	// cubemap

uniform int RenderMode = 0;

void main()
{
	if(RenderMode == 0)			//regular colormap rendering
	{
		outputColor = texture(texture_data, outUV); 
	}
	else if(RenderMode == 1)	//cubemap rendering 1st version
	{
		outputColor = texture(cubemap_data, CubeMapTexCoord);
	}
	else if(RenderMode == 2)	//cubemap rendering 2nd version
	{

	}
	else if(RenderMode == 3)	//cubemap rendering Refraction
	{

	}
}