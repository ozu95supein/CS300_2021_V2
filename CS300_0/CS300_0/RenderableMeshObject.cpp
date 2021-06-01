#include "RenderableMeshObject.h"
RenderableMeshObject::RenderableMeshObject() : mObjectVBO ( 0 )
{
    mObjectVBO                  = -1;
    mObjectVAO                  = -1;
    mObjectNormal_VBO           = -1;
    mObjectNormal_VAO           = -1;
    mObjectAveragedNormal_VBO   = -1;
    mObjectAveragedNormal_VAO   = -1;
}
RenderableMeshObject::RenderableMeshObject(MeshType t, int slices, GLuint& vbo, GLuint& vao, GLuint& Normal_vbo, GLuint& Normal_vao, GLuint& Averaged_vbo, GLuint& Averaged_vao)
{
    mObjectMesh = Mesh();
    switch (t)
    {
    case MeshType::PLANE:
        mObjectMesh.ConstructPlane();
        break;
    case MeshType::CUBE:
        mObjectMesh.ConstructCube();
        break;
    case MeshType::CYLINDER:
        mObjectMesh.ConstructCylinder(slices);
        break;
    case MeshType::CONE:
        mObjectMesh.ConstructCone(slices);
        break;
    case MeshType::SPHERE:
        mObjectMesh.ConstructCylinder(slices);
        break;
    }
    mObjectVBO = vbo;
    mObjectVAO = vao;
    mObjectNormal_VBO = Normal_vbo;
    mObjectNormal_VAO = Normal_vao;
    mObjectAveragedNormal_VBO = Averaged_vbo;
    mObjectAveragedNormal_VAO = Averaged_vao;
    //generateNormal lines for the Mesh Object
    mObjectMesh.GenerateNormalLines();
    mObjectMesh.GenerateAveragedNormalLines();
    //initialize buffers for vbo and vaos
    Renderable_InitAllBuffers();
}
RenderableMeshObject::~RenderableMeshObject()
{
    Renderable_CleanUpObjectAndBuffers(mObjectVBO, mObjectVAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectNormal_VBO, mObjectNormal_VAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectAveragedNormal_VBO, mObjectAveragedNormal_VAO, mObjectMesh);
    mObjectMesh.~Mesh();
}

void RenderableMeshObject::Renderable_InitAllBuffers()
{
    Renderable_InitializeMeshBuffers(mObjectVBO, mObjectVAO, mObjectMesh);
    Renderable_InitializeNormalBuffers(mObjectNormal_VBO, mObjectNormal_VAO, mObjectMesh);
    Renderable_InitializeAveragedNormalBuffers(mObjectAveragedNormal_VBO, mObjectAveragedNormal_VAO, mObjectMesh);
}
void RenderableMeshObject::Renderable_InitializeMeshBuffers(GLuint vbo, GLuint vao, Mesh& mesh)
{
    unsigned long stride = sizeof(Vertex);
    // create buffer for VAO
    glGenVertexArrays(1, &vao);
    // create buffer for VBO
    glGenBuffers(1, &vbo);
    //bind so we are now doing stuff to the vao
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(Vertex) * mesh.GetVertexNum()), mesh.GetVertices(), GL_STATIC_DRAW);

    //positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, 0);
    //normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, normal)));
    //uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, UV)));

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void RenderableMeshObject::Renderable_InitializeNormalBuffers(GLuint vbo, GLuint vao, Mesh& mesh)
{
    unsigned long stride = sizeof(glm::vec4);
    // create buffer for VAO
    glGenVertexArrays(1, &vao);
    // create buffer for VBO
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(Mesh::NormalLine) * mesh.GetNormalsNum()), mesh.GetNormals(), GL_STATIC_DRAW);

    //start
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, 0);
}
void RenderableMeshObject::Renderable_InitializeAveragedNormalBuffers(GLuint vbo, GLuint vao, Mesh& mesh)
{
    unsigned long stride = sizeof(glm::vec4);
    // create buffer for VAO
    glGenVertexArrays(1, &vao);
    // create buffer for VBO
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(Mesh::NormalLine) * mesh.GetNormalsNum()), mesh.GetAveragedNormals(), GL_STATIC_DRAW);

    //start
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, 0);
}

Mesh& RenderableMeshObject::GetMesh()
{
    return mObjectMesh;
}
GLuint& RenderableMeshObject::GetVBO()
{
    return mObjectVBO;
}
GLuint& RenderableMeshObject::GetVAO()
{
    return mObjectVAO;
}
GLuint& RenderableMeshObject::GetNormalVBO()
{
    return mObjectNormal_VBO;
}
GLuint& RenderableMeshObject::GetNormalVAO()
{
    return mObjectNormal_VAO;
}
GLuint& RenderableMeshObject::GetAveragedNormalVBO()
{
    return mObjectAveragedNormal_VBO;
}
GLuint& RenderableMeshObject::GetAveragedNormalVAO()
{
    return mObjectAveragedNormal_VAO;
}
void RenderableMeshObject::Renderable_CleanUpObjectAndBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh)
{
    // Delete the VBOs
    glDeleteBuffers(1, &vbo);
    // Delete the VAO
    glDeleteVertexArrays(1, &vao);
    mesh.CleanupAndReset();
}