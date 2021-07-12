#include "LightSourceObject.h"
LightSourceObject::LightSourceObject()
{
	mRenderable = RenderableMeshObject();
	mLight = Light();
}
LightSourceObject::~LightSourceObject()
{

}
LightSourceObject::LightSourceObject(RenderableMeshObject R, Light L, glm::mat4 S, glm::mat4 T)
{
	glm::mat4 M = T * S;
	RenderableMeshObject LIGHT_sphereObject(MeshType::SPHERE, 6, M);
	mRenderable = LIGHT_sphereObject;
	mLight = L;
	mScaleMatrix = S;
}
void LightSourceObject::SetModelMatrix(glm::mat4 M)
{
	mRenderable.SetModel(M);
}
glm::mat4 LightSourceObject::GetModelMatrix()
{
	return mRenderable.GetModel();
}
glm::vec4 LightSourceObject::GetLightPosition()
{
	return mLight.light_position;
}
void LightSourceObject::SetLightPosition(glm::vec4 pos)
{
	mLight.light_position = pos;
}
void LightSourceObject::TranslateLightSourceObject(glm::vec4 v)
{
	glm::mat4 LIGHT_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(v.x, v.y, v.z));
	glm::mat4 LIGHT_ModelMatrix = LIGHT_translationMatrix * mScaleMatrix;
	mRenderable.SetModel(LIGHT_ModelMatrix);
	mLight.light_position = v;
}