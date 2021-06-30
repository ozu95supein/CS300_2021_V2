#version 400
out vec4 outputColor;

uniform sampler2D depth_texture_data; // smallColors.png
in vec2 outUV;
float contrast = 0.999999999999f;
void main()
{
	outputColor = texture(depth_texture_data, outUV);
	outputColor.xyz = (outputColor.xyz - 0.5) * (1.0f + contrast) + 0.5f;
}