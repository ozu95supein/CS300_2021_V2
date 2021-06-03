#pragma once
#include "glm/glm.hpp"
using glm::vec3;

struct LightSourceParameters
{
	vec3 lightPosition = vec3(0.0f);
	vec3 lightDirection = vec3(1.0f);
	vec3 ambient = vec3(0.5f);
	vec3 diffuse = vec3(0.5f);
	vec3 specular = vec3(1.0f);
	vec3 attenuation = vec3(0.0f, 0.0f, 0.005f);
	float inner = 30.0f;
	float outer = 35.0f;
	float falloff = 1.0f;
};
 