#version 400
out vec4 outputColor;
in vec2 outUV;

uniform sampler2D texture_data; // Grass.png

void main()
{
    vec4 meshColor4 = texture(texture_data, outUV); 
    if (meshColor4.w < 0.8f)
      discard; 

    outputColor = meshColor4;
}