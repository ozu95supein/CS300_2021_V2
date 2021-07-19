#pragma once
#include "OGLDebug.h"
#include "ShaderUtils.h"

#include "Mesh.h"
#include "Light_And_Material.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

//Auxiliary variables for rendering in different modes
struct AuxRenderVariables
{
    int display_wiremesh;
    int RenderMode;
    int UsingFaceNormals;
    int using_shadows; 
};
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

    void Renderable_InitializeTangentBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh);
    void Renderable_InitializeAveragedTangentBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh);

    void Renderable_InitializeBiTangentBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh);
    void Renderable_InitializeAveragedBiTangentBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh);

    Mesh & GetMesh();
    GLuint& GetVBO();
    GLuint& GetVAO();
    GLuint& GetNormalVBO();
    GLuint& GetNormalVAO();
    GLuint& GetAveragedNormalVBO();
    GLuint& GetAveragedNormalVAO();

    void Renderable_CleanUpObjectAndBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh);
    void Renderable_SetLightingUniforms(GLuint& shader, Light& CurrentLight, Material& CurrentMaterial, glm::mat4& LightViewMatrix, glm::mat4& LightProjectionMatrix, bool using_shadows, int neighbor);
    void Renderable_firstPass(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& depthshader, const int ShadowMapWidth, const int ShadowMapHeight);
    void Renderable_secondPass(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& shader, GLuint& texture, bool display_wiremesh, int RenderMode, Light& CurrentLight, GLuint& NormalMap, int UsingFaceNormals, GLuint&depthTex, glm::mat4& LightViewMatrix, glm::mat4& LightProjectionMatrix, bool using_shadows, int neighbor);
    void Renderable_displayNormals(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& normalShader);
    void Renderable_displayAveragedNormals(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& normalShader);
    void Renderable_displayTangents(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& tangentShader);
    void Renderable_displayAveragedTangents(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& tangentShader);
    void Renderable_displayBiTangents(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& bitangentShader);
    void Renderable_displayAveragedBiTangents(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& bitangentShader);
    void Renderable_ChangeSlices(int new_slices, MeshType& t);

    void Renderable_displayDepth(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& depthplaneshader, GLuint& texture);

    glm::mat4& GetModelRefference();
    glm::mat4 GetModel();
    void SetModel(glm::mat4 m);
    void SetMaterial(Material m);
    void Translate(glm::vec3 newPosition);
    Material& GetMaterialRefference();
    void Renderable_RotateModel(float rotation_radians, glm::vec3 axis);

    void New_Renderable_MainDisplay(Camera camera, Light& CurrentLight, Camera light_camera, AuxRenderVariables variables);

    //shader and texture
    void SetObjectTexture(GLuint t)
    {
        mObjectTexture = t;
    }
    void SetObjectNormalMap(GLuint n)
    {
        mObjectNormalMapTexture = n;
    }
    void SetegularShaderProgram(GLuint s)
    {
        mRegularShaderProgram = s;
    }

private:

    Mesh mObjectMesh;
    GLuint mObjectVBO;
    GLuint mObjectVAO;

    GLuint mObjectNormal_VBO;
    GLuint mObjectNormal_VAO;
    GLuint mObjectAveragedNormal_VBO;
    GLuint mObjectAveragedNormal_VAO;

    GLuint mObjectTangent_VBO;
    GLuint mObjectTangent_VAO;
    GLuint mObjectAveragedTangent_VBO;
    GLuint mObjectAveragedTangent_VAO;

    GLuint mObjectBiTangent_VBO;
    GLuint mObjectBiTangent_VAO;
    GLuint mObjectAveragedBiTangent_VBO;
    GLuint mObjectAveragedBiTangent_VAO;

    glm::mat4 mModelMatrix;
    Material mMaterial;

    GLuint mObjectTexture;  //texture specific to this object
    GLuint mObjectNormalMapTexture;  //texture specific to this object
    GLuint mRegularShaderProgram;  //for regular rendering as well as acting as the second pass for shadows
};
