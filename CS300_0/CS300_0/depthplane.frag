#version 400
out vec4 outputColor;

uniform sampler2D texture_data; // smallColors.png
in vec2 outUV;
in vec4 position_modelSpace;

void main()
{
	outputColor = texture(texture_data, outUV);
}