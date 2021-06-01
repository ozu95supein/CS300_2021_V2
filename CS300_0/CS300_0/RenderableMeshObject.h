#pragma once
#include "OGLDebug.h"
#include "ShaderUtils.h"

#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class RenderableMeshObject
{
public:
    RenderableMeshObject();
    ~RenderableMeshObject();
    RenderableMeshObject(MeshType t, int slices);
    
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
private:

    Mesh mObjectMesh;
    GLuint mObjectVBO;
    GLuint mObjectVAO;
    GLuint mObjectNormal_VBO;
    GLuint mObjectNormal_VAO;
    GLuint mObjectAveragedNormal_VBO;
    GLuint mObjectAveragedNormal_VAO;
};
