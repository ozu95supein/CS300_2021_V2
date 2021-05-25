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

enum class MeshType {PLANE = 0, CUBE = 1, CYLINDER = 2, CONE = 3, SPHERE = 4};

class Mesh
{

public:
	struct NormalLine
	{
		NormalLine(glm::vec4 s, glm::vec4 e)
		{
			start = s;
			end = e;
		}
		glm::vec4 start;
		glm::vec4 end;
	};
	Mesh();
	~Mesh();
	void ConstructPlane();
	void ConstructCube();
	void ConstructCylinder(int slices);
	void ConstructCone(int slices);
	void ConstructSphere(int slices);
	void CleanupAndReset();
	void* GetVertices();
	void* GetNormals();
	int GetVertexNum();
	int GetNormalsNum();
	void SetVertexNum(int v);

	int GetFaceNum();
	void SetFaceNum(int f);

	MeshType GetType();
	void SetType(MeshType t);


	void GenerateNormalLines();
private:
	std::vector<Vertex> mVertexArray;
	std::vector<NormalLine> mNormalArray;
	int mVertexNum;
	int mFaceNum;
	MeshType mType;
};