#include "InstanceMesh.h"

InstanceMesh::InstanceMesh()
{
	ConstructPlane();

}
InstanceMesh::~InstanceMesh()
{
	CleanupAndReset();
}
void InstanceMesh::ConstructPlane()
{
	//stored attributes counter clockwise
	glm::vec4 p0 = glm::vec4(-0.5, 0.5, 0, 1);
	glm::vec4 p1 = glm::vec4(-0.5, -0.5, 0, 1);
	glm::vec4 p2 = glm::vec4(0.5, -0.5, 0, 1);
	glm::vec4 p3 = glm::vec4(0.5, 0.5, 0, 1);

	glm::vec4 n0 = glm::vec4(0.0f, 0.0f, 1.0f, 0);
	glm::vec4 n1 = glm::vec4(0.0f, 0.0f, 1.0f, 0);
	glm::vec4 n2 = glm::vec4(0.0f, 0.0f, 1.0f, 0);
	glm::vec4 n3 = glm::vec4(0.0f, 0.0f, 1.0f, 0);

	glm::vec2 uv0 = glm::vec2(0.0, 0.0);
	glm::vec2 uv1 = glm::vec2(0.0, 1.0);
	glm::vec2 uv2 = glm::vec2(1.0, 1.0);
	glm::vec2 uv3 = glm::vec2(1.0, 0.0);

	Vertex v0 = Vertex(p0, n0, uv0);
	Vertex v1 = Vertex(p1, n1, uv1);
	Vertex v2 = Vertex(p2, n2, uv2);
	Vertex v3 = Vertex(p3, n3, uv3);

	SetVertexNum(6);
	SetFaceNum(2);
	//first face v0, v1, v2
	mVertexArray.push_back(v0);
	mVertexArray.push_back(v1);
	mVertexArray.push_back(v2);
	//second face vo, v2, v3
	mVertexArray.push_back(v3);
	mVertexArray.push_back(v0);
	mVertexArray.push_back(v2);
}
void InstanceMesh::CleanupAndReset()
{
	mVertexArray.clear();
	mVertexNum = 0;
	mFaceNum = 0;
}
void* InstanceMesh::GetVertices()
{
	//IS THIS OK?
	auto ptr = &(*(mVertexArray.begin()));
	return (void*)(ptr);
}
int InstanceMesh::GetVertexNum()
{
	return mVertexNum;
}
void InstanceMesh::SetVertexNum(int v)
{
	mVertexNum = v;
}
int InstanceMesh::GetFaceNum()
{
	return mFaceNum;
}
void InstanceMesh::SetFaceNum(int f)
{
	mFaceNum = f;
}