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
    RenderableMeshObject(MeshType t, int slices, GLuint& vbo, GLuint& vao, Mesh& mesh, GLuint& Normal_vbo, GLuint& Normal_vao, GLuint& Averaged_vbo, GLuint& Averaged_vao);
    
    void Renderable_InitAllBuffers();
    void Renderable_InitializeMeshBuffers(GLuint vbo, GLuint vao, Mesh & mesh);
    void Renderable_InitializeNormalBuffers(GLuint vbo, GLuint vao, Mesh & mesh);
    void Renderable_InitializeAveragedNormalBuffers(GLuint vbo, GLuint vao, Mesh & mesh);

private:

    Mesh mObjectMesh;
    GLuint mObjectVBO;
    GLuint mObjectVAO;
    GLuint mObjectNormal_VBO;
    GLuint mObjectNormal_VAO;
    GLuint mObjectAveragedNormal_VBO;
    GLuint mObjectAveragedNormal_VAO;
};
