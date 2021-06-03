#pragma once
#include "glm/glm.hpp"

struct MaterialParameters
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float ns;	//shininess
	MaterialParameters()
	{
		ambient = glm::vec3(1.0f);
		diffuse = glm::vec3(1.0f);
		specular = glm::vec3(1.0);
		ns = 10.0f;
	}
};