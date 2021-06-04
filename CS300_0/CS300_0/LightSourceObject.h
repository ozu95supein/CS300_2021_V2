#pragma once
#include "RenderableMeshObject.h"
struct LightSourceObject
{
	glm::mat4 model;
	Light mLight;
	RenderableMeshObject mLightSphere;
};