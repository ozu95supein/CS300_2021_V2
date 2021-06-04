#pragma once
#include <glm/glm.hpp>

struct Light
{
    glm::vec3 light_ambient;
    glm::vec3 light_diffuse;
    glm::vec3 light_specular;
    glm::vec4 light_position;    //MAKE IT THE SAME AS THE SPHERE OBJECT FOR NOW
};
struct Material
{
    glm::vec3 material_ambient;
    glm::vec3 material_diffuse;
    glm::vec3 material_specular;
};
