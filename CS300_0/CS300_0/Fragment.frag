#version 400
out vec4 outputColor;
in vec2 outUV;
in vec4 position_cameraspace;
in vec4 normal_cameraspace;
in vec4 lightPosition_cameraspace;

uniform sampler2D texture_data; // smallColors.png
uniform int texture_toggle;

uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialSpecularNS;

void main()
{
	vec4 L = lightPosition_cameraspace - position_cameraspace;
	if(texture_toggle == 1)
	{
		vec4 meshColor4 = texture(texture_data, outUV); 
		vec3 meshColor3 = vec3(meshColor4.x, meshColor4.y, meshColor4.z);
		vec3 K_a = meshColor3 * materialAmbient;
		vec3 I_ambient = lightAmbient * K_a;
		
		vec3 K_d = meshColor3 * materialDiffuse;
		float NL_result = dot(normal_cameraspace, L);
		NL_result = max(NL_result, 0.0f);
		vec3 I_diffuse = lightDiffuse * K_d * NL_result;

		vec3 K_s = meshColor3 * materialSpecular;
		//calculate R as a vec4
		vec4 R4 = ((2 * NL_result) * normal_cameraspace) - L;
		vec4 cameraView_cannonical = vec4(0.0f, 0.0f, -1.0f, 0.0f);
		float RV_Result = dot(R4, cameraView_cannonical);
		RV_Result = max(RV_Result, 0.0f);
		float RV_NS_Result = pow(RV_Result, materialSpecularNS);
		vec3 I_specular = lightSpecular * K_s * RV_NS_Result;


		vec3 I_total = I_ambient + I_diffuse + I_specular;
		outputColor = vec4(I_total, 1.0f);

	}
	else
	{
		vec3 meshColor3 = vec3(outUV.x, outUV.y, 0);
		vec3 K_a = meshColor3 * materialAmbient;
		vec3 I_ambient = lightAmbient * K_a;

		vec3 K_d = meshColor3 * materialDiffuse;
		float NL_result = dot(normal_cameraspace, L);
		NL_result = max(NL_result, 0.0f);
		vec3 I_diffuse = lightDiffuse * K_d * NL_result;

		vec3 K_s = meshColor3 * materialSpecular;
		//calculate R as a vec4
		vec4 R4 = ((2 * NL_result) * normal_cameraspace) - L;
		vec4 cameraView_cannonical = vec4(0.0f, 0.0f, -1.0f, 0.0f);
		float RV_Result = dot(R4, cameraView_cannonical);
		RV_Result = max(RV_Result, 0.0f);
		float RV_NS_Result = pow(RV_Result, materialSpecularNS);
		vec3 I_specular = lightSpecular * K_s * RV_NS_Result;

		vec3 I_total = I_ambient + I_diffuse + I_specular;
		outputColor = vec4(I_total, 1.0f);
	}
}