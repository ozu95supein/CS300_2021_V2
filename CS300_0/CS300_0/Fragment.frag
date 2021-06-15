#version 400
out vec4 outputColor;
in vec2 outUV;
in vec4 position_cameraspace;
in vec4 lightPosition_cameraspace;

in vec4 normal_cameraspace;
in vec4 Avg_normal_camerapsace;
in vec4 Tangent_Cameraspace;
in vec4 BiTangent_Cameraspace;
in vec4 Avg_Tangent_Cameraspace;
in vec4 Avg_BiTangent_Cameraspace;
in mat4 TBN_mat;
in mat4 Avg_TBN_mat;

uniform sampler2D texture_data; // smallColors.png
uniform sampler2D normalMap_data;
uniform int texture_toggle;
uniform int faceNormal_toggle;

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
	vec4 NormalMap4 = texture(normalMap_data, outUV); 
	vec3 NormalMap3 = vec3(NormalMap4.x, NormalMap4.y, NormalMap4.z);	

	float nx = (NormalMap3.x * 2.0f) - 1.0f;
	float ny = (NormalMap3.y * 2.0f) - 1.0f;
	float nz = (NormalMap3.z * 2.0f) - 1.0f;

	vec4 newNormal4 = vec4(nx, ny, nz, 0.0f);
	vec4 TBN_Normal = TBN_mat * newNormal4;
	vec4 AvgTBN_Normal = Avg_TBN_mat * newNormal4;
	//debug
	//outputColor = TBN_Normal;
	if(texture_toggle == 1)
	{
		if(faceNormal_toggle == 1)
		{
			vec4 meshColor4 = texture(texture_data, outUV); 
			vec3 meshColor3 = vec3(meshColor4.x, meshColor4.y, meshColor4.z);
			vec3 K_a = meshColor3 * materialAmbient;
			vec3 I_ambient = lightAmbient * K_a;
			
			vec3 K_d = meshColor3 * materialDiffuse;
			float NL_result = dot(TBN_Normal, L);
			NL_result = max(NL_result, 0.0f);
			vec3 I_diffuse = lightDiffuse * K_d * NL_result;
	
			vec3 K_s = materialSpecular;
			//calculate R as a vec4
			vec4 R4 = ((2 * NL_result) * TBN_Normal) - L;
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
			vec4 meshColor4 = texture(texture_data, outUV); 
			vec3 meshColor3 = vec3(meshColor4.x, meshColor4.y, meshColor4.z);
			vec3 K_a = meshColor3 * materialAmbient;
			vec3 I_ambient = lightAmbient * K_a;
			
			vec3 K_d = meshColor3 * materialDiffuse;
			float NL_result = dot(AvgTBN_Normal, L);
			NL_result = max(NL_result, 0.0f);
			vec3 I_diffuse = lightDiffuse * K_d * NL_result;
	
			vec3 K_s = materialSpecular;
			//calculate R as a vec4
			vec4 R4 = ((2 * NL_result) * AvgTBN_Normal) - L;
			vec4 cameraView_cannonical = vec4(0.0f, 0.0f, -1.0f, 0.0f);
			float RV_Result = dot(R4, cameraView_cannonical);
			RV_Result = max(RV_Result, 0.0f);
			float RV_NS_Result = pow(RV_Result, materialSpecularNS);
			vec3 I_specular = lightSpecular * K_s * RV_NS_Result;
	
			vec3 I_total = I_ambient + I_diffuse + I_specular;
			outputColor = vec4(I_total, 1.0f);
		}
	}
	else
	{
		if(faceNormal_toggle == 1)
		{
			vec3 meshColor3 = vec3(outUV.x, outUV.y, 0);
			vec3 K_a = meshColor3 * materialAmbient;
			vec3 I_ambient = lightAmbient * K_a;
	
			vec3 K_d = meshColor3 * materialDiffuse;
			float NL_result = dot(TBN_Normal, L);
			NL_result = max(NL_result, 0.0f);
			vec3 I_diffuse = lightDiffuse * K_d * NL_result;
	
			vec3 K_s = materialSpecular;
			//calculate R as a vec4
			vec4 R4 = ((2 * NL_result) * TBN_Normal) - L;
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
			float NL_result = dot(AvgTBN_Normal, L);
			NL_result = max(NL_result, 0.0f);
			vec3 I_diffuse = lightDiffuse * K_d * NL_result;
	
			vec3 K_s = materialSpecular;
			//calculate R as a vec4
			vec4 R4 = ((2 * NL_result) * AvgTBN_Normal) - L;
			vec4 cameraView_cannonical = vec4(0.0f, 0.0f, -1.0f, 0.0f);
			float RV_Result = dot(R4, cameraView_cannonical);
			RV_Result = max(RV_Result, 0.0f);
			float RV_NS_Result = pow(RV_Result, materialSpecularNS);
			vec3 I_specular = lightSpecular * K_s * RV_NS_Result;
	
			vec3 I_total = I_ambient + I_diffuse + I_specular;
			outputColor = vec4(I_total, 1.0f);
		}
		
	}
}