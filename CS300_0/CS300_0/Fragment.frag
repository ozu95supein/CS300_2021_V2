#version 400

// struct definition
struct Light
{
	vec3 lightPosition = vec3(0.0f);
	vec3 lightDirection = vec3(1.0f);
    vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.5f);
	vec3 specular = vec3(1.0f);
	vec3 attenuation = vec3(0.0f, 0.0f, 0.005f);
	float inner = 30.0f;
	float outer = 35.0f;
	float falloff = 1.0f;
};
struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float ns;	//shininess
}

out vec4 outputColor;
in vec2 outUV;
in position_cameraspace;
in normal_cameraspace;

uniform sampler2D texture_data; // smallColors.png
uniform int texture_toggle;

uniform Light u_Light;
uniform Material u_Material;
void main()
{
	//lighting Computations
	//vec3 I_ambient = I_a * K_a;
	//vec3 I_diffuse = I_d * K_d * max((dot(N, L)), 0.0f));
	//vec3 I_specular = (I_s * K_s * max((dot(N, H), 0.0f)))

	//only doing ambient for now
	vec3 I_a = u_Light.ambient;
	vec3 k_a = u_Material.ambient;

	vec3 I_ambient = I_a * 0.1f;
	if(texture_toggle == 1)
	{
		outputColor = texture(texture_data, outUV); 
		outputColor = outputColor + I_ambient;
	}
	else
	{
		outputColor = vec4(vec3(outUV.x, outUV.y, 0), 1.0f);
		outputColor = outputColor + I_ambient;
	}
	
}