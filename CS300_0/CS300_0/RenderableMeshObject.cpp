#include "RenderableMeshObject.h"
RenderableMeshObject::RenderableMeshObject()
{

}
RenderableMeshObject::RenderableMeshObject(MeshType t, int slices, GLuint& vbo, GLuint& vao, Mesh& mesh, GLuint& Normal_vbo, GLuint& Normal_vao, GLuint& Averaged_vbo, GLuint& Averaged_vao)
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

    mObjectMesh.GenerateNormalLines();
    mObjectMesh.GenerateAveragedNormalLines();
    InitializeMeshBuffers(mObjectVBO, mObjectVAO, mObjectMesh);
    InitializeNormalBuffers(mObjectNormal_VBO, mObjectNormal_VAO, mObjectMesh);
    InitializeAveragedNormalBuffers(mObjectAveragedNormal_VBO, mObjectAveragedNormal_VAO, mObjectMesh);
}