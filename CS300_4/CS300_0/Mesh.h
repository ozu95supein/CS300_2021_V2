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
		AveragedNormal = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		tangents = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		bi_tangents = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		AveragedNormal_tangents = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		AveragedNormal_bi_tangents = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 UV;
	glm::vec4 AveragedNormal;
	glm::vec4 tangents;
	glm::vec4 bi_tangents;
	glm::vec4 AveragedNormal_tangents;
	glm::vec4 AveragedNormal_bi_tangents;
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
	void* GetAveragedNormals();

	void* GetTangents();
	void* GetAveragedTangents();

	void* GetBiTangents();
	void* GetAveragedBiTangents();

	int GetVertexNum();
	int GetNormalsNum();
	void SetVertexNum(int v);

	int GetFaceNum();
	void SetFaceNum(int f);

	MeshType GetType();
	void SetType(MeshType t);


	void GenerateNormalLines();

	void ConstructAveragedNormals();
	void GenerateAveragedNormalLines();
	void GenerateTangentLines();
	void GenerateAveragedTangentLines();
	void GenerateBiTangentLines();
	void GenerateAveragedBiTangentLines();

	void AveragedNormal_ComputeTangentBasis();
	void ComputeTangetBasis();

private:
	std::vector<Vertex> mVertexArray;
	std::vector<NormalLine> mNormalArray;
	std::vector<NormalLine> mAveragedNormalArray;
	std::vector<NormalLine> mTangentArray;
	std::vector<NormalLine> mAveragedTangentArray;
	std::vector<NormalLine> mBiTangentArray;
	std::vector<NormalLine> mAveragedBiTangentArray;
	int mVertexNum;
	int mFaceNum;
	MeshType mType;
};