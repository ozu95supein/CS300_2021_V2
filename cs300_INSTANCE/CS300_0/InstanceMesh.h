#pragma once
#include "glm/glm.hpp"
#include <vector>

struct Vertex
{
    Vertex(glm::vec4 p, glm::vec4 n, glm::vec2 uv)
    {
        position = p;
        normal = n;
        UV = uv;
    }
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec2 UV;
};
class InstanceMesh
{
public:
    InstanceMesh();
    ~InstanceMesh();
    void ConstructPlane();
    void CleanupAndReset();
    void* GetVertices();
    int GetVertexNum();
    void SetVertexNum(int v);
    int GetFaceNum();
    void SetFaceNum(int f);
    
private:
    std::vector<Vertex> mVertexArray;
    int mVertexNum;
    int mFaceNum;
};
