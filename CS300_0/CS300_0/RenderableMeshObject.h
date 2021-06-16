#pragma once
#include "OGLDebug.h"
#include "ShaderUtils.h"

#include "Mesh.h"
#include "Light_And_Material.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class RenderableMeshObject
{
public:
    RenderableMeshObject();
    ~RenderableMeshObject();
    RenderableMeshObject(MeshType t, int slices, glm::mat4 ModelMatrix);
    
    void Renderable_InitAllBuffers();
    void Renderable_InitializeMeshBuffers(GLuint & vbo, GLuint & vao, Mesh & mesh);
    void Renderable_InitializeNormalBuffers(GLuint & vbo, GLuint & vao, Mesh & mesh);
    void Renderable_InitializeAveragedNormalBuffers(GLuint & vbo, GLuint & vao, Mesh & mesh);

    Mesh & GetMesh();
    GLuint& GetVBO();
    GLuint& GetVAO();
    GLuint& GetNormalVBO();
    GLuint& GetNormalVAO();
    GLuint& GetAveragedNormalVBO();
    GLuint& GetAveragedNormalVAO();

    void Renderable_CleanUpObjectAndBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh);
    void Renderable_SetLightingUniforms(GLuint& shader, Light& CurrentLight, Material& CurrentMaterial);
    void Renderable_displayMesh(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& shader, GLuint& texture, bool display_wiremesh, int RenderMode, Light& CurrentLight, GLuint& NormalMap, int UsingFaceNormals);
    void Renderable_displayNormals(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& normalShader);
    void Renderable_displayAveragedNormals(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& normalShader);
    void Renderable_ChangeSlices(int new_slices, MeshType& t);
    glm::mat4& GetModelRefference();
    glm::mat4 GetModel();
    void SetModel(glm::mat4 m);
    void SetMaterial(Material m);
    void Translate(glm::vec3 newPosition);
    Material& GetMaterialRefference();
private:

    Mesh mObjectMesh;
    GLuint mObjectVBO;
    GLuint mObjectVAO;
    GLuint mObjectNormal_VBO;
    GLuint mObjectNormal_VAO;
    GLuint mObjectAveragedNormal_VBO;
    GLuint mObjectAveragedNormal_VAO;
    glm::mat4 mModelMatrix;
    Material mMaterial;
};
