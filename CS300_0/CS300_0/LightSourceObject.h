#pragma once
#include "RenderableMeshObject.h"
struct LightSourceObject
{
	LightSourceObject(glm::mat4& m, Light& l, RenderableMeshObject& sph) 
	{
		model = m;
		mLight = l;
		mLightSphere = sph;
	}
	void SetLightSourceModel(glm::mat4 m)
	{
		model = m;
	}
	glm::mat4 GetLightSourceModel()
	{
		return model;
	}
	void SetLightSourceLightAttributes(Light l)
	{
		mLight = l;
	}
	Light GetLightSourceAttributes()
	{
		return mLight;
	}
	void SetLightSourceSphere(RenderableMeshObject sph)
	{
		mLightSphere = sph;
	}
	RenderableMeshObject GetLightSourceSphere()
	{
		return mLightSphere;
	}
	void TranslateEntireLightSource(glm::vec3 TranslationPosition, glm::vec3 LookAtPoint)
	{
		model = glm::translate(model, TranslationPosition);
		mLight.light_position = glm::vec4(TranslationPosition, 1.0f);
		mLightSphere.Translate(TranslationPosition);
	}
	glm::mat4 model;
	Light mLight;
	RenderableMeshObject mLightSphere;
};