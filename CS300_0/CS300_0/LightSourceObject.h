#pragma once
#include "RenderableMeshObject.h"

class LightSourceObject
{
public:
	LightSourceObject();
	LightSourceObject(RenderableMeshObject R, Light L, glm::mat4 S, glm::mat4 T);
	~LightSourceObject();
	void SetModelMatrix(glm::mat4 M);
	glm::mat4 GetModelMatrix();
	glm::vec4 GetLightPosition();
	void SetLightPosition(glm::vec4 pos);
	void TranslateLightSourceObject(glm::vec4 v);
private:
	glm::mat4 mScaleMatrix;
	RenderableMeshObject mRenderable;
	Light mLight;
};