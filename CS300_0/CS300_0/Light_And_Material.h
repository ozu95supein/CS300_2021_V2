#pragma once
#include <glm/glm.hpp>

struct Light
{
    glm::vec3 light_ambient;
    glm::vec3 light_diffuse;
    glm::vec3 light_specular;
    glm::vec3 light_attenuation;
    glm::vec4 light_position;    //MAKE IT THE SAME AS THE SPHERE OBJECT FOR NOW
    int light_type;                 //0 = PointLight, 1 = SpotLight, 2 = DirectionalLight
    float inner = 30.0f;
    float outer = 35.0f;
    float falloff = 1.0f;
};
struct Material
{
    glm::vec3 material_ambient;
    glm::vec3 material_diffuse;
    glm::vec3 material_specular;
    float ns;
};
