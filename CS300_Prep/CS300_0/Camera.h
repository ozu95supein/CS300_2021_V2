#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
class Camera
{
public:
	Camera()
	{
		mViewMatrix = mat4(1.0f);
		mProjectionMatrix = mat4(1.0f);
	}
	~Camera()
	{

	}
	Camera(mat4 V, mat4 P)
	{
		mViewMatrix = V;
		mProjectionMatrix = P;
	}
	Camera(vec3 eye_pos, vec3 up, vec3 position_to_look_at, float FOV, float aspect, float near, float far)
	{
		mViewMatrix = lookAt(eye_pos, position_to_look_at, up);
		mProjectionMatrix = perspective(FOV, aspect, near, far);
	}
	mat4 GetViewMatrix()
	{
		return mViewMatrix;
	}
	void SetViewMatrix(mat4 v)
	{
		mViewMatrix = v;
	}
	mat4 GetProjectionMatrix()
	{
		return mProjectionMatrix;
	}
	void SetProjectionMatrix(mat4 p)
	{
		mProjectionMatrix = p;
	}
	vec3 ExtractCamPosition()
	{
		vec3 result;
		result.x = mViewMatrix[0].w;
		result.y = mViewMatrix[1].w;
		result.z = mViewMatrix[2].w;
		return result;
	}
private:
	mat4 mViewMatrix;
	mat4 mProjectionMatrix;
};