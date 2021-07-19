#version 400
out vec4 outputColor;

uniform sampler2D depth_texture_data; // smallColors.png
in vec2 outUV;
float contrast = 0.99f;
void main()
{
	float r = ((texture(depth_texture_data, outUV).r - contrast) / (1.0f - contrast));
	float g = ((texture(depth_texture_data, outUV).g - contrast) / (1.0f - contrast));
	float b = ((texture(depth_texture_data, outUV).b - contrast) / (1.0f - contrast));
	vec3 frag3 = vec3(r, g, b);
	outputColor = vec4(frag3, 1.0f);
}