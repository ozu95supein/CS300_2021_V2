#include "RenderableMeshObject.h"
RenderableMeshObject::RenderableMeshObject() : mObjectVBO ( 0 )
{
    mObjectVBO                  = -1;
    mObjectVAO                  = -1;
    mObjectNormal_VBO           = -1;
    mObjectNormal_VAO           = -1;
    mObjectAveragedNormal_VBO   = -1;
    mObjectAveragedNormal_VAO   = -1;
    mModelMatrix = glm::mat4(1.0f);
}
RenderableMeshObject::RenderableMeshObject(MeshType t, int slices, glm::mat4 ModelMatrix)
{
    mModelMatrix = ModelMatrix;
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
        mObjectMesh.ConstructSphere(slices);
        break;
    }
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
void RenderableMeshObject::Renderable_InitializeMeshBuffers(GLuint &vbo, GLuint &vao, Mesh& mesh)
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
void RenderableMeshObject::Renderable_InitializeNormalBuffers(GLuint & vbo, GLuint & vao, Mesh& mesh)
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
void RenderableMeshObject::Renderable_InitializeAveragedNormalBuffers(GLuint & vbo, GLuint & vao, Mesh& mesh)
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

void RenderableMeshObject::Renderable_SetLightingUniforms(GLuint& shader, Light& CurrentLight, Material& CurrentMaterial)
{
    glUseProgram(shader);
    //pass them to program
    
    //AMBIENT
    GLint LIGHTAMBIENT = glGetUniformLocation(shader, "lightAmbient");
    glUniform3fv(LIGHTAMBIENT, 1, &(CurrentLight.light_ambient[0]));
    GLint MATERIALAMBIENT = glGetUniformLocation(shader, "materialAmbient");
    glUniform3fv(MATERIALAMBIENT, 1, &(CurrentMaterial.material_ambient[0]));

    //DIFFUSE

}
void RenderableMeshObject::Renderable_displayMesh(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& shader, GLuint& texture, bool display_wiremesh, int ColoredBoxTextureOn, Light & CurrentLight, Material & CurrentMaterial)
{
    ////////////////////////////////////////////////////////////////////////////////
        // Bind the glsl program and this object's VAO
    glUseProgram(shader);
    //pass them to program
    GLint model = glGetUniformLocation(shader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(shader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(shader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    Renderable_SetLightingUniforms(shader, CurrentLight, CurrentMaterial);

    //ColoredBoxTextureOn
    GLuint texture_tog = glGetUniformLocation(shader, "texture_toggle");
    glUniform1i(texture_tog, ColoredBoxTextureOn);

    //texture stuff
    glActiveTexture(GL_TEXTURE0); //activate bucket 0
    glBindTexture(GL_TEXTURE_2D, texture);  //fill bucket 0
    GLuint loc = glGetUniformLocation(shader, "texture_data");   //get uniform of frag shader
    glUniform1i(loc, 0);    //use stuff from bucket 0

    // Draw
    if (display_wiremesh == false)
    {
        glBindVertexArray(mObjectVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, mObjectMesh.GetVertexNum());
    }
    else
    {
        glBindVertexArray(mObjectVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, mObjectMesh.GetVertexNum());
    }
}
void RenderableMeshObject::Renderable_displayNormals(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& normalShader)
{
    // Bind the glsl program and this object's VAO
    glUseProgram(normalShader);

    //pass them to program
    GLint model = glGetUniformLocation(normalShader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(normalShader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(normalShader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    glBindVertexArray(mObjectNormal_VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int s = 2 * mObjectMesh.GetVertexNum();
    glDrawArrays(GL_LINES, 0, s);
}
void RenderableMeshObject::Renderable_displayAveragedNormals(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& normalShader)
{
    // Bind the glsl program and this object's VAO
    glUseProgram(normalShader);

    //pass them to program
    GLint model = glGetUniformLocation(normalShader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(normalShader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(normalShader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    glBindVertexArray(mObjectAveragedNormal_VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int s = 2 * mObjectMesh.GetVertexNum();
    glDrawArrays(GL_LINES, 0, s);
}
glm::mat4& RenderableMeshObject::GetModelRefference()
{
    return mModelMatrix;
}
glm::mat4 RenderableMeshObject::GetModel()
{
    return mModelMatrix;
}
void RenderableMeshObject::SetModel(glm::mat4 m)
{
    mModelMatrix = m;
}
void RenderableMeshObject::Renderable_ChangeSlices(int new_slices, MeshType& t)
{
    //reset the Mesh object and clean up the buffers
    Renderable_CleanUpObjectAndBuffers(mObjectVBO, mObjectVAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectNormal_VBO, mObjectNormal_VAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectAveragedNormal_VBO, mObjectAveragedNormal_VAO, mObjectMesh);

    //reconstruct mesh with new slices
    switch (t)
    {
    case MeshType::CYLINDER:
        mObjectMesh.ConstructCylinder(new_slices);
        break;
    case MeshType::CONE:
        mObjectMesh.ConstructCone(new_slices);
        break;
    case MeshType::SPHERE:
        mObjectMesh.ConstructSphere(new_slices);
        break;
    default:
        break;
    }

    Renderable_InitializeMeshBuffers(mObjectVBO, mObjectVAO, mObjectMesh);
    mObjectMesh.GenerateNormalLines();
    mObjectMesh.GenerateAveragedNormalLines();
    Renderable_InitializeNormalBuffers(mObjectNormal_VBO, mObjectNormal_VAO, mObjectMesh);
    Renderable_InitializeAveragedNormalBuffers(mObjectAveragedNormal_VBO, mObjectAveragedNormal_VAO, mObjectMesh);
}