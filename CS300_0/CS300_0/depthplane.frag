#version 400
out vec4 outputColor;

uniform sampler2D depth_texture_data; // smallColors.png
in vec2 outUV;
float contrast = 0.6f;
void main()
{
	vec4 sample4 = texture(depth_texture_data, outUV);
	// sample4.xyz /= sample4.a;
	// // Apply contrast.
	// sample4.xyz = ((sample4.xyz - 0.5f) * max(contrast, 0.0f)) + 0.5f;
	// sample4.xyz *= sample4.a;
	// // pixelColor.rgb = ((pixelColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f;
	outputColor = vec4(sample4.xyz, 1.0f);
}