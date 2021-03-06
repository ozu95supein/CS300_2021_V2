#version 400
out vec4 outputColor;
in vec2 outUV;
in vec4 position_cameraspace;
in vec4 position_lightspace;
in vec4 lightPosition_cameraspace;
in vec4 lightDirection_cameraspace;

in vec4 normal_cameraspace;
in vec4 Avg_normal_camerapsace;
in vec4 Tangent_Cameraspace;
in vec4 BiTangent_Cameraspace;
in vec4 Avg_Tangent_Cameraspace;
in vec4 Avg_BiTangent_Cameraspace;


uniform sampler2D texture_data; // smallColors.png
uniform sampler2D normalMap_data;
uniform sampler2D shadowMap_data;

uniform int Render_Mode;
uniform int faceNormal_toggle;
uniform int light_type;
uniform int using_shadows_int;
uniform int u_neighbors;

uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform vec3 lightAttenuation;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialSpecularNS;
uniform float lightInner;
uniform float lightOuter;
uniform float lightFalloff;

float getPCFShadow(vec2 UV, vec3 position_lightspace_NDC3)
{
	vec2 texelOffset = 1.0 / textureSize(shadowMap_data, 0);
	int neighbor = u_neighbors; // For a 5x5 neighborhood
	float accumulatedVisibility = 0.0f;
	float sampleCount = 0.0f;
	for(int x = -neighbor; x <= neighbor; x++)
	{
		for(int y = -neighbor; y <= neighbor; y++)
		{
			float shadowDepth = texture(shadowMap_data, UV + texelOffset * vec2(x, y)).r;
			// Evaluate visibility for this sample
			if(position_lightspace_NDC3.z < shadowDepth)
			{
				accumulatedVisibility += 1.0f;
			}
			sampleCount += 1.0f;
		}
	}
	return accumulatedVisibility / sampleCount;
}

void main()
{
	vec3 temp_normal_cameraspace = normal_cameraspace.xyz;
	vec3 NORMALIZED_normal_cameraspace = normalize(temp_normal_cameraspace);

	vec3 temp_Avg_normal_camerapsace = Avg_normal_camerapsace.xyz;
	vec3 NORMALIZED_Avg_normal_camerapsace = normalize(temp_Avg_normal_camerapsace);

	vec3 temp_Tangent_Cameraspace = Tangent_Cameraspace.xyz;
	vec3 NORMALIZED_Tangent_Cameraspace = normalize(temp_Tangent_Cameraspace);

	vec3 temp_BiTangent_Cameraspace = BiTangent_Cameraspace.xyz;
	vec3 NORMALIZED_BiTangent_Cameraspace = normalize(temp_BiTangent_Cameraspace);

	vec3 temp_Avg_Tangent_Cameraspace = Avg_Tangent_Cameraspace.xyz;
	vec3 NORMALIZED_Avg_Tangent_Cameraspace = normalize(temp_Avg_Tangent_Cameraspace);

	vec3 temp_Avg_BiTangent_Cameraspace = Avg_BiTangent_Cameraspace.xyz;
	vec3 NORMALIZED_Avg_BiTangent_Cameraspace = normalize(temp_Avg_BiTangent_Cameraspace);

	//make the matrices that go from tangent space to cameraspace
	mat3 TBN_mat3 = mat3(NORMALIZED_Tangent_Cameraspace, NORMALIZED_BiTangent_Cameraspace, NORMALIZED_normal_cameraspace);
	mat3 Avg_TBN_mat3 = mat3(NORMALIZED_Avg_Tangent_Cameraspace, NORMALIZED_Avg_BiTangent_Cameraspace, NORMALIZED_Avg_normal_camerapsace);

	vec3 L = (lightPosition_cameraspace - position_cameraspace).xyz;
	vec3 D = lightDirection_cameraspace.xyz;
	vec3 NORMALIZED_D = normalize(D);
	vec3 NORMALIZED_L = normalize(L);
	vec3 NormalMap4 = texture(normalMap_data, outUV).xyz; 
	
	float nx = (NormalMap4.x * 2.0f) - 1.0f;
	float ny = (NormalMap4.y * 2.0f) - 1.0f;
	float nz = (NormalMap4.z * 2.0f) - 1.0f;

	vec3 newNormal3 = vec3(nx, ny, nz);
	vec3 TBN_Normal = TBN_mat3 * newNormal3;
	vec3 AvgTBN_Normal = Avg_TBN_mat3 * newNormal3;
	vec3 NORMALIZED_TBN_Normal = normalize(TBN_Normal);
	vec3 NORMALIZED_AvgTBN_Normal = normalize(AvgTBN_Normal);

	float dl = L.length();
	float att_denominator = lightAttenuation.x + lightAttenuation.y * dl + (lightAttenuation.y * pow(dl, 2));
	float att = min(1.0f/att_denominator, 1.0f);
	
	float SpotlightEffect = 1.0f;

	//SHADOWMAP
	//Do perspective division
	float w = position_lightspace.w;
	vec3 position_lightspace_persDiv = position_lightspace.xyz / w;;
	vec3 position_lightspace_NDC = (position_lightspace_persDiv * 0.5f) + 0.5f;

	vec2 shadowUV = vec2(position_lightspace_NDC.x, position_lightspace_NDC.y);
	vec3 ShadowMap3 = texture(shadowMap_data, shadowUV).xyz; 

	float shadowmap_mod;

	if(using_shadows_int == 1)
	{
		shadowmap_mod = getPCFShadow(shadowUV, position_lightspace_NDC);
	}
	else
	{
		shadowmap_mod = 1.0f;
	}
	
	if(light_type == 1)	//spotlight
	{
		vec3 neg_L =  - NORMALIZED_L;
		float var = (dot(neg_L, NORMALIZED_D));
		float alpha_rad = acos(var);
		float theta_rad = radians(lightInner);
		float phi_rad = radians(lightOuter);
		SpotlightEffect = pow(((cos(alpha_rad) - cos(phi_rad)) / (cos(theta_rad) - cos(phi_rad))), lightFalloff);
		SpotlightEffect = clamp(SpotlightEffect, 0.0f, 1.0f);
	}
	if(light_type == 2) //directional light_type{
	{
		//attenuation is not considered when dealing with directional light
		att = 1.0f;
		//in directional light L will be equal to -D
		NORMALIZED_L = -NORMALIZED_D;
	}
	
	if(Render_Mode == 0)
	{
		if(faceNormal_toggle == 1)
		{
			//Ambient
			vec4 meshColor4 = texture(texture_data, outUV); 
			vec3 meshColor3 = vec3(meshColor4.x, meshColor4.y, meshColor4.z);
			vec3 K_a = meshColor3 * materialAmbient;
			vec3 I_ambient = lightAmbient * K_a;
			//Diffuse
			vec3 K_d = meshColor3 * materialDiffuse;
			float NL_result = dot(NORMALIZED_TBN_Normal, NORMALIZED_L);
			NL_result = max(NL_result, 0.0f);
			vec3 I_diffuse = lightDiffuse * K_d * NL_result;
			//Specular
			vec3 K_s = materialSpecular;
			//calculate R as a vec4
			vec3 R3 = 2.0f * (dot(NORMALIZED_TBN_Normal, NORMALIZED_L)) * NORMALIZED_TBN_Normal - NORMALIZED_L;
			vec3 NORMALIZED_R3 = normalize(R3);
			vec3 ViewVector = vec3(0.0f, 0.0f, 0.0f) - position_cameraspace.xyz;
			vec3 NORMALIZED_ViewVector = normalize(ViewVector);
			float RV_Result = dot(NORMALIZED_R3, NORMALIZED_ViewVector);
			RV_Result = max(RV_Result, 0.0f);
			float RV_NS_Result = pow(RV_Result, materialSpecularNS);
			vec3 I_specular = lightSpecular * K_s * RV_NS_Result;
			
			vec3 I_total = att * (I_ambient + shadowmap_mod *SpotlightEffect * (I_diffuse + I_specular));
			outputColor = vec4(I_total, 1.0f);
		}
		else
		{
			//Ambient
			vec4 meshColor4 = texture(texture_data, outUV); 
			vec3 meshColor3 = vec3(meshColor4.x, meshColor4.y, meshColor4.z);
			vec3 K_a = meshColor3 * materialAmbient;
			vec3 I_ambient = lightAmbient * K_a;
			//Diffuse
			vec3 K_d = meshColor3 * materialDiffuse;
			float NL_result = dot(NORMALIZED_AvgTBN_Normal, NORMALIZED_L);
			NL_result = max(NL_result, 0.0f);
			vec3 I_diffuse = lightDiffuse * K_d * NL_result;
			//Specular
			vec3 K_s = materialSpecular;
			//calculate R as a vec4
			vec3 R3 = 2.0f * (dot(NORMALIZED_AvgTBN_Normal, NORMALIZED_L)) * NORMALIZED_AvgTBN_Normal - NORMALIZED_L;
			vec3 NORMALIZED_R3 = normalize(R3);
			vec3 ViewVector = vec3(0.0f, 0.0f, 0.0f) - position_cameraspace.xyz;
			vec3 NORMALIZED_ViewVector = normalize(ViewVector);
			float RV_Result = dot(NORMALIZED_R3, NORMALIZED_ViewVector);
			RV_Result = max(RV_Result, 0.0f);
			float RV_NS_Result = pow(RV_Result, materialSpecularNS);
			vec3 I_specular = lightSpecular * K_s * RV_NS_Result;
			
			vec3 I_total = att * (I_ambient + shadowmap_mod *SpotlightEffect * (I_diffuse + I_specular));
			outputColor = vec4(I_total, 1.0f);
		}
	}
	else if(Render_Mode == 1)
	{
		outputColor = vec4(normalize(normal_cameraspace).xyz, 1.0f);
	}
	else if(Render_Mode == 2)
	{
		outputColor = vec4(normalize(Tangent_Cameraspace).xyz, 1.0f);
	}
	else if(Render_Mode == 3)
	{
		outputColor = vec4(normalize(BiTangent_Cameraspace).xyz, 1.0f);
	}

}