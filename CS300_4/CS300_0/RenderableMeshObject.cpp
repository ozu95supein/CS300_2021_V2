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
    mObjectMesh.GenerateTangentLines();
    mObjectMesh.GenerateAveragedTangentLines();
    mObjectMesh.GenerateBiTangentLines();
    mObjectMesh.GenerateAveragedBiTangentLines();

    //initialize buffers for vbo and vaos
    Renderable_InitAllBuffers();
}
RenderableMeshObject::~RenderableMeshObject()
{
    Renderable_CleanUpObjectAndBuffers(mObjectVBO, mObjectVAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectNormal_VBO, mObjectNormal_VAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectAveragedNormal_VBO, mObjectAveragedNormal_VAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectTangent_VBO, mObjectTangent_VAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectAveragedTangent_VBO, mObjectAveragedTangent_VAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectBiTangent_VBO, mObjectBiTangent_VAO, mObjectMesh);
    Renderable_CleanUpObjectAndBuffers(mObjectAveragedBiTangent_VBO, mObjectAveragedBiTangent_VAO, mObjectMesh);
    mObjectMesh.~Mesh();
}
void RenderableMeshObject::Renderable_InitAllBuffers()
{
    Renderable_InitializeMeshBuffers(mObjectVBO, mObjectVAO, mObjectMesh);
    Renderable_InitializeNormalBuffers(mObjectNormal_VBO, mObjectNormal_VAO, mObjectMesh);
    Renderable_InitializeAveragedNormalBuffers(mObjectAveragedNormal_VBO, mObjectAveragedNormal_VAO, mObjectMesh);
    Renderable_InitializeTangentBuffers(mObjectTangent_VBO, mObjectTangent_VAO, mObjectMesh);
    Renderable_InitializeAveragedTangentBuffers(mObjectAveragedTangent_VBO, mObjectAveragedTangent_VAO, mObjectMesh);
    Renderable_InitializeBiTangentBuffers(mObjectBiTangent_VBO, mObjectBiTangent_VAO, mObjectMesh);
    Renderable_InitializeAveragedBiTangentBuffers(mObjectAveragedBiTangent_VBO, mObjectAveragedBiTangent_VAO, mObjectMesh);
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
    //aAvgNormal
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, AveragedNormal)));
    //aTangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, tangents)));
    //aBiTangent
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, bi_tangents)));
    //aAvgTangent
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, AveragedNormal_tangents)));
    //aAvgBiTangent
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, AveragedNormal_bi_tangents)));

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
void RenderableMeshObject::Renderable_InitializeAveragedNormalBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh)
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
void RenderableMeshObject::Renderable_InitializeTangentBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh)
{
    unsigned long stride = sizeof(glm::vec4);
    // create buffer for VAO
    glGenVertexArrays(1, &vao);
    // create buffer for VBO
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(Mesh::NormalLine) * mesh.GetNormalsNum()), mesh.GetTangents(), GL_STATIC_DRAW);

    //start
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, 0);
}
void RenderableMeshObject::Renderable_InitializeAveragedTangentBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh)
{
    unsigned long stride = sizeof(glm::vec4);
    // create buffer for VAO
    glGenVertexArrays(1, &vao);
    // create buffer for VBO
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(Mesh::NormalLine) * mesh.GetNormalsNum()), mesh.GetAveragedTangents(), GL_STATIC_DRAW);

    //start
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, 0);
}
void RenderableMeshObject::Renderable_InitializeBiTangentBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh)
{
    unsigned long stride = sizeof(glm::vec4);
    // create buffer for VAO
    glGenVertexArrays(1, &vao);
    // create buffer for VBO
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(Mesh::NormalLine) * mesh.GetNormalsNum()), mesh.GetBiTangents(), GL_STATIC_DRAW);

    //start
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, 0);
}
void RenderableMeshObject::Renderable_InitializeAveragedBiTangentBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh)
{
    unsigned long stride = sizeof(glm::vec4);
    // create buffer for VAO
    glGenVertexArrays(1, &vao);
    // create buffer for VBO
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(Mesh::NormalLine) * mesh.GetNormalsNum()), mesh.GetAveragedBiTangents(), GL_STATIC_DRAW);

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
    GLuint LIGHTDIFFUSE = glGetUniformLocation(shader, "lightDiffuse");
    glUniform3fv(LIGHTDIFFUSE, 1, &(CurrentLight.light_diffuse[0]));
    GLuint LIGHTPOSITION = glGetUniformLocation(shader, "lightPosition");
    glUniform4fv(LIGHTPOSITION, 1, &(CurrentLight.light_position[0]));
    GLuint MATERIALDIFFUSE = glGetUniformLocation(shader, "materialDiffuse");
    glUniform3fv(MATERIALDIFFUSE, 1, &(CurrentMaterial.material_diffuse[0]));

    //SPECULAR
    GLuint LIGHTSPECULAR = glGetUniformLocation(shader, "lightSpecular");
    glUniform3fv(LIGHTSPECULAR, 1, &(CurrentLight.light_specular[0]));
    GLuint MATERIALSPECULAR = glGetUniformLocation(shader, "materialSpecular");
    glUniform3fv(MATERIALSPECULAR, 1, &(CurrentMaterial.material_specular[0]));
    GLuint MATERIALSPECULAR_NS = glGetUniformLocation(shader, "materialSpecularNS");
    glUniform1f(MATERIALSPECULAR_NS, (CurrentMaterial.ns));

    //ATTENUATION
    GLuint LIGHTATTENUATION = glGetUniformLocation(shader, "lightAttenuation");
    glUniform3fv(LIGHTATTENUATION, 1, &(CurrentLight.light_attenuation[0]));

    //LIGHTSOURCETYPE
    GLuint LIGHTSOURCETYPE = glGetUniformLocation(shader, "light_type");
    glUniform1i(LIGHTSOURCETYPE, CurrentLight.light_type);

    //SPOTLIGHT
    GLuint LIGHTINNERANGLE = glGetUniformLocation(shader, "lightInner");
    glUniform1f(LIGHTINNERANGLE, (CurrentLight.inner));
    GLuint LIGHTOUTERANGLE = glGetUniformLocation(shader, "lightOuter");
    glUniform1f(LIGHTOUTERANGLE, (CurrentLight.outer));
    GLuint LIGHTFALLOFF = glGetUniformLocation(shader, "lightFalloff");
    glUniform1f(LIGHTFALLOFF, (CurrentLight.falloff));

    //DIRECTION
    GLuint LIGHTDIRECTION = glGetUniformLocation(shader, "lightDirection");
    glUniform4fv(LIGHTDIRECTION, 1, &(CurrentLight.light_direction[0]));
}
void RenderableMeshObject::Renderable_displayMesh(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& shader, GLuint& texture, bool display_wiremesh, int RenderMode, Light & CurrentLight, GLuint& NormalMap, int UsingFaceNormals)
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

    Renderable_SetLightingUniforms(shader, CurrentLight, mMaterial);

    //ColoredBoxTextureOn
    GLuint texture_tog = glGetUniformLocation(shader, "Render_Mode");
    glUniform1i(texture_tog, RenderMode);

    GLuint normalmap_tog = glGetUniformLocation(shader, "faceNormal_toggle");
    glUniform1i(normalmap_tog, UsingFaceNormals);

    //texture stuff
    glActiveTexture(GL_TEXTURE0); //activate bucket 0
    glBindTexture(GL_TEXTURE_2D, texture);  //fill bucket 0
    GLuint loc = glGetUniformLocation(shader, "texture_data");   //get uniform of frag shader
    glUniform1i(loc, 0);    //use stuff from bucket 0

    glActiveTexture(GL_TEXTURE1); //activate bucket 1
    glBindTexture(GL_TEXTURE_2D, NormalMap);
    GLuint loc1 = glGetUniformLocation(shader, "normalMap_data");   //get uniform of frag shader
    glUniform1i(loc1, 1);    //use stuff from bucket 1

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
void RenderableMeshObject::Renderable_displayTangents(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& tangentShader)
{
    // Bind the glsl program and this object's VAO
    glUseProgram(tangentShader);

    //pass them to program
    GLint model = glGetUniformLocation(tangentShader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(tangentShader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(tangentShader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    glBindVertexArray(mObjectTangent_VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int s = 2 * mObjectMesh.GetVertexNum();
    glDrawArrays(GL_LINES, 0, s);
}
void RenderableMeshObject::Renderable_displayAveragedTangents(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& tangentShader)
{
    // Bind the glsl program and this object's VAO
    glUseProgram(tangentShader);

    //pass them to program
    GLint model = glGetUniformLocation(tangentShader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(tangentShader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(tangentShader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    glBindVertexArray(mObjectAveragedBiTangent_VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int s = 2 * mObjectMesh.GetVertexNum();
    glDrawArrays(GL_LINES, 0, s);
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
void RenderableMeshObject::Renderable_displayBiTangents(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& bitangentShader)
{
    // Bind the glsl program and this object's VAO
    glUseProgram(bitangentShader);

    //pass them to program
    GLint model = glGetUniformLocation(bitangentShader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(bitangentShader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(bitangentShader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    glBindVertexArray(mObjectBiTangent_VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int s = 2 * mObjectMesh.GetVertexNum();
    glDrawArrays(GL_LINES, 0, s);
}
void RenderableMeshObject::Renderable_displayAveragedBiTangents(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& bitangentShader)
{
    // Bind the glsl program and this object's VAO
    glUseProgram(bitangentShader);

    //pass them to program
    GLint model = glGetUniformLocation(bitangentShader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(bitangentShader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(bitangentShader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    glBindVertexArray(mObjectAveragedBiTangent_VAO);
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
void RenderableMeshObject::SetMaterial(Material m)
{
    mMaterial = m;
}
void RenderableMeshObject::Translate(glm::vec3 newPosition)
{
    mModelMatrix = glm::translate(mModelMatrix, newPosition);
}
Material& RenderableMeshObject::GetMaterialRefference()
{
    return mMaterial;
}

void RenderableMeshObject::Renderable_displayCubeMap(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& shader, GLuint& CubemapTexture)
{
    // Bind the glsl program and this object's VAO
    glUseProgram(shader);
    //pass them to program
    GLint model = glGetUniformLocation(shader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(shader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(shader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    //texture stuff
    glActiveTexture(GL_TEXTURE0); //activate bucket 0
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);  //fill bucket 0
    GLuint loc = glGetUniformLocation(shader, "skyBox_data");   //get uniform of frag shader
    glUniform1i(loc, 0);    //use stuff from bucket 0

    glBindVertexArray(mObjectVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, mObjectMesh.GetVertexNum());
}

void RenderableMeshObject::Renderable_DisplayToFBO(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& CubeMapShader, GLuint& CubeMapFaceTexture, int RenderMode)
{
    // Bind the glsl program and this object's VAO
    glUseProgram(CubeMapShader);
    // Enable front-face culling
    glCullFace(GL_BACK);
    //pass them to program
    GLint model = glGetUniformLocation(CubeMapShader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(CubeMapShader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(CubeMapShader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    //ColoredBoxTextureOn
    GLuint texture_tog = glGetUniformLocation(CubeMapShader, "texture_toggle");
    glUniform1i(texture_tog, 1);

    //RenderMode
    GLuint RenderMode_tog = glGetUniformLocation(CubeMapShader, "RenderMode");
    glUniform1i(RenderMode_tog, RenderMode);

    //draw depth buffer
    glBindVertexArray(mObjectVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, mObjectMesh.GetVertexNum());
}
void RenderableMeshObject::Renderable_DisplayMultiRenderMode(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& MultiRenderShader, GLuint& ColorMaptexture, GLuint& CubeMapFaceTexture, int RenderMode, glm::vec3 cam_pos, int using_facenormals, float refraction_ratio)
{
    // Bind the glsl program and this object's VAO
    glUseProgram(MultiRenderShader);
    // Enable front-face culling
    glCullFace(GL_BACK);
    //pass them to program
    GLint model = glGetUniformLocation(MultiRenderShader, "u_M");
    glUniformMatrix4fv(model, 1, GL_FALSE, &(mModelMatrix[0][0]));
    GLint view = glGetUniformLocation(MultiRenderShader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(MultiRenderShader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));
    
    //RenderMode
    GLuint RenderMode_tog = glGetUniformLocation(MultiRenderShader, "RenderMode");
    glUniform1i(RenderMode_tog, RenderMode);

    //using face normals
    GLuint FaceNormal_tog = glGetUniformLocation(MultiRenderShader, "usingFaceNormals");
    glUniform1i(FaceNormal_tog, using_facenormals);

    //ratio
    GLuint RefractionRatio = glGetUniformLocation(MultiRenderShader, "refraction_index_ratio");
    glUniform1i(RefractionRatio, refraction_ratio);

    //texture stuff
    glActiveTexture(GL_TEXTURE0); //activate bucket 0
    glBindTexture(GL_TEXTURE_2D, ColorMaptexture);  //fill bucket 0
    GLuint loc = glGetUniformLocation(MultiRenderShader, "texture_data");   //get uniform of frag shader
    glUniform1i(loc, 0);    //use stuff from bucket 0

    // cubemap
    glActiveTexture(GL_TEXTURE1); //activate bucket 1
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapFaceTexture);
    GLuint loc1 = glGetUniformLocation(MultiRenderShader, "cubemap_data");   //get uniform of frag shader
    glUniform1i(loc1, 1);    //use stuff from bucket 1
    
    glm::vec4 cam4 = glm::vec4(cam_pos, 0.0f);
    GLint camPos = glGetUniformLocation(MultiRenderShader, "camera_position");
    glUniform4fv(camPos, 1, &(cam4[0]));

    //Draw Triangles
    glBindVertexArray(mObjectVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, mObjectMesh.GetVertexNum());


}