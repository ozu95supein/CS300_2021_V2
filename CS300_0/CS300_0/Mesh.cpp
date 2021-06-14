#include "Mesh.h"
#include <iostream>
float PI = glm::atan(1) * 4;
Mesh::Mesh() : mVertexNum{ 0 }, mFaceNum{ 0 }, mType{ MeshType::PLANE }, mVertexArray{ std::vector<Vertex>() }, mNormalArray{ std::vector<NormalLine>() }
{
}
Mesh::~Mesh()
{
	CleanupAndReset();
}

void Mesh::ConstructPlane()
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

	SetType(MeshType::PLANE);
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

	ConstructAveragedNormals();
}
void Mesh::ConstructCube()
{
	//stored attributes counter clockwise
	//front face
	glm::vec4 p0 = glm::vec4(-0.5, 0.5, 0.5, 1);
	glm::vec4 p1 = glm::vec4(-0.5, -0.5, 0.5, 1);
	glm::vec4 p2 = glm::vec4(0.5, -0.5, 0.5, 1);
	glm::vec4 p3 = glm::vec4(0.5, 0.5, 0.5, 1);
	//back face
	glm::vec4 p4 = glm::vec4(-0.5, 0.5, -0.5, 1);
	glm::vec4 p5 = glm::vec4(-0.5, -0.5, -0.5, 1);
	glm::vec4 p6 = glm::vec4(0.5, -0.5, -0.5, 1);
	glm::vec4 p7 = glm::vec4(0.5, 0.5, -0.5, 1);

	//front
	glm::vec4 N_front = glm::vec4(0.0f, 0.0f, 1.0f, 0);
	//right
	glm::vec4 N_right = glm::vec4(1.0f, 0.0f, 0.0f, 0);
	//back
	glm::vec4 N_back = glm::vec4(0.0f, 0.0f, -1.0f, 0);
	//left
	glm::vec4 N_left = glm::vec4(-1.0f, 0.0f, 0.0f, 0);
	//top
	glm::vec4 N_top = glm::vec4(0.0f, 1.0f, 0.0f, 0);
	//bottom
	glm::vec4 N_bottom = glm::vec4(0.0f, -1.0f, 0.0f, 0);
	//UVs
	glm::vec2 uv0 = glm::vec2(0.0, 0.0);
	glm::vec2 uv1 = glm::vec2(0.0, 1.0);
	glm::vec2 uv2 = glm::vec2(1.0, 1.0);
	glm::vec2 uv3 = glm::vec2(1.0, 0.0);

	//front
	Vertex v0 = Vertex(p0, N_front, uv0);
	Vertex v1 = Vertex(p1, N_front, uv1);
	Vertex v2 = Vertex(p3, N_front, uv3);

	Vertex v3 = Vertex(p1, N_front, uv1);
	Vertex v4 = Vertex(p2, N_front, uv2);
	Vertex v5 = Vertex(p3, N_front, uv3);

	//right
	//top left
	Vertex v6 = Vertex(p3, N_right, uv0);
	Vertex v7 = Vertex(p2, N_right, uv1);
	Vertex v8 = Vertex(p7, N_right, uv3);
	//bottom right
	Vertex v9 = Vertex(p2, N_right, uv1);
	Vertex v10 = Vertex(p6, N_right, uv2);
	Vertex v11 = Vertex(p7, N_right, uv3);

	//back
	//top left
	Vertex v12 = Vertex(p7, N_back, uv0);
	Vertex v13 = Vertex(p6, N_back, uv1);
	Vertex v14 = Vertex(p4, N_back, uv3);

	Vertex v15 = Vertex(p6, N_back, uv1);
	Vertex v16 = Vertex(p5, N_back, uv2);
	Vertex v17 = Vertex(p4, N_back, uv3);

	//left
	Vertex v18 = Vertex(p4, N_left, uv0);
	Vertex v19 = Vertex(p5, N_left, uv1);
	Vertex v20 = Vertex(p0, N_left, uv3);

	Vertex v21 = Vertex(p5, N_left, uv1);
	Vertex v22 = Vertex(p1, N_left, uv2);
	Vertex v23 = Vertex(p0, N_left, uv3);

	//top
	Vertex v24 = Vertex(p4, N_top, uv0);
	Vertex v25 = Vertex(p0, N_top, uv1);
	Vertex v26 = Vertex(p7, N_top, uv3);

	Vertex v27 = Vertex(p0, N_top, uv1);
	Vertex v28 = Vertex(p3, N_top, uv2);
	Vertex v29 = Vertex(p7, N_top, uv3);

	//bottom
	Vertex v30 = Vertex(p1, N_bottom, uv0);
	Vertex v31 = Vertex(p5, N_bottom, uv1);
	Vertex v32 = Vertex(p2, N_bottom, uv3);

	Vertex v33 = Vertex(p5, N_bottom, uv1);
	Vertex v34 = Vertex(p6, N_bottom, uv2);
	Vertex v35 = Vertex(p2, N_bottom, uv3);

	SetType(MeshType::CUBE);
	SetVertexNum(36);
	SetFaceNum(12);

	mVertexArray.push_back(v0);
	mVertexArray.push_back(v1);
	mVertexArray.push_back(v2);
	mVertexArray.push_back(v3);
	mVertexArray.push_back(v4);
	mVertexArray.push_back(v5);
	mVertexArray.push_back(v6);
	mVertexArray.push_back(v7);
	mVertexArray.push_back(v8);
	mVertexArray.push_back(v9);
	mVertexArray.push_back(v10);
	mVertexArray.push_back(v11);
	mVertexArray.push_back(v12);
	mVertexArray.push_back(v13);
	mVertexArray.push_back(v14);
	mVertexArray.push_back(v15);
	mVertexArray.push_back(v16);
	mVertexArray.push_back(v17);
	mVertexArray.push_back(v18);
	mVertexArray.push_back(v19);
	mVertexArray.push_back(v20);
	mVertexArray.push_back(v21);
	mVertexArray.push_back(v22);
	mVertexArray.push_back(v23);
	mVertexArray.push_back(v24);
	mVertexArray.push_back(v25);
	mVertexArray.push_back(v26);
	mVertexArray.push_back(v27);
	mVertexArray.push_back(v28);
	mVertexArray.push_back(v29);
	mVertexArray.push_back(v30);
	mVertexArray.push_back(v31);
	mVertexArray.push_back(v32);
	mVertexArray.push_back(v33);
	mVertexArray.push_back(v34);
	mVertexArray.push_back(v35);
	std::cout << "CUBE" << std::endl;
	ConstructAveragedNormals();
}
void Mesh::ConstructCylinder(int slices)
{
	//std::cout << "MAKING CYLINDER" << std::endl;
	std::vector<glm::vec4> positions;
	std::vector<float> U_coords;
	//center vertex, top and Bottom
	glm::vec4 p0(0.0f, 0.5f, 0.0f, 1.0f);
	glm::vec4 p1(0.0f, -0.5f, 0.0f, 1.0f);
	positions.push_back(p0);
	positions.push_back(p1);
	float angle_rad = 0.0f;
	float a = 360.0f / slices;
	float rad_increment = glm::radians(a);
	
	//TODO, MAKE NORMALS
	//top
	glm::vec4 N_top = glm::vec4(0.0f, 1.f, 0.0f, 0);
	//bottom
	glm::vec4 N_bottom = glm::vec4(0.0f, -1.0f, 0.0f, 0);
	std::vector<glm::vec4> NormalsForSides;
	//make the top-bottom pairs of all positions on the same y axis
	for (int i = 0; i < slices; i++)
	{
		//radius is == 0.5f
		float x = 0.5f * glm::cos(angle_rad);
		float z = 0.5f * glm::sin(angle_rad);
		glm::vec4 up(x, 0.5f, z, 1.0f);
		glm::vec4 down(x, -0.5f, z, 1.0f);
		//std::cout << "Angle: " << angle_rad << std::endl;
		//std::cout << up.x << ", "<< up.y << ", "<< up.z << ", "<< up.w << std::endl;
		//std::cout << down.x << ", "<< down.y << ", "<< down.z << ", "<< down.w << std::endl;
		positions.push_back(up);
		positions.push_back(down);

		float u = angle_rad / (2 * PI);
		U_coords.push_back(u);

		angle_rad += rad_increment;
		//U coord calculations
		//TODO make PI better
		
	}
	//NOTE ORDER has to be CENTER SECOND FIRST Vert due to weirdness in XZ plane to
	// 	   make this entire thing counterclockwise
	//top faces of the cylinder
	for (int i = 0; i < slices; i++)
	{
		//this is for final iteration
		if (i == (slices - 1))
		{
			glm::vec4 center = positions[0];
			int a = 2 + (2 * i);
			glm::vec4 fistVert = positions[a];
			int b = 2;
			glm::vec4 secondVert = positions[b];

			mVertexArray.push_back(Vertex(center, N_top, glm::vec2(U_coords[i], 1.0f)));
			mVertexArray.push_back(Vertex(secondVert, N_top, glm::vec2(1.0f, 1.0f)));
			mVertexArray.push_back(Vertex(fistVert, N_top, glm::vec2(U_coords[i], 1.0f)));

		}
		else
		{
			glm::vec4 center = positions[0];
			int a = 2 + (2 * i);
			glm::vec4 fistVert = positions[a];
			int b = 4 + (2 * i);
			glm::vec4 secondVert = positions[b];

			mVertexArray.push_back(Vertex(center, N_top, glm::vec2(U_coords[i], 1.0f)));
			int k = i + 1;
			mVertexArray.push_back(Vertex(secondVert, N_top, glm::vec2(U_coords[k], 1.0f)));
			mVertexArray.push_back(Vertex(fistVert, N_top, glm::vec2(U_coords[i], 1.0f)));

		}
	}
	//side faces of the cylinder
	for (int i = 0; i < slices; i++)
	{
		//this is for final iteration
		if (i == (slices - 1))
		{
			int a = 2;
			glm::vec4 topleft(positions[a]);
			int b = 3;
			glm::vec4 bottomleft(positions[b]);
			int c = 2 + 2 * i;
			glm::vec4 topright(positions[c]);
			int d = c + 1;
			glm::vec4 bottomright(positions[d]);

			//make normals through cross product
			glm::vec3 v_1 = (bottomleft - topleft);
			glm::vec3 v_2 = (topright - topleft);
			glm::vec3 v_3 = glm::cross(v_1, v_2);
			glm::vec3 v_4 = glm::normalize(v_3);
			float l = glm::length(v_4);
			glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);

			//top left triangle
			mVertexArray.push_back(Vertex(topright, n, glm::vec2(U_coords[i], 1.0f)));
			mVertexArray.push_back(Vertex(topleft, n, glm::vec2(1.0f, 1.0f)));
			mVertexArray.push_back(Vertex(bottomleft, n, glm::vec2(1.0f, 0.0f)));
			//bottom right triangle
			mVertexArray.push_back(Vertex(topright, n, glm::vec2(U_coords[i], 1.0f)));
			mVertexArray.push_back(Vertex(bottomleft, n, glm::vec2(1.0f, 0.0f)));
			mVertexArray.push_back(Vertex(bottomright, n, glm::vec2(U_coords[i], 0.0f)));
		}
		else
		{
			int a = 4 + 2 * i;
			glm::vec4 topleft(positions[a]);
			int b = a + 1;
			glm::vec4 bottomleft(positions[b]);
			int c = 2 + 2 * i;
			glm::vec4 topright(positions[c]);
			int d = c + 1;
			glm::vec4 bottomright(positions[d]);

			//make normals through cross product
			glm::vec3 v_1 = (bottomleft - topleft);
			glm::vec3 v_2 = (topright - topleft);
			glm::vec3 v_3 = glm::cross(v_1, v_2);
			glm::vec3 v_4 = glm::normalize(v_3);
			glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);



			//top left triangle
			mVertexArray.push_back(Vertex(topright, n, glm::vec2(U_coords[i], 1.0f)));
			int k = i + 1;
			mVertexArray.push_back(Vertex(topleft, n, glm::vec2(U_coords[k], 1.0f)));
			mVertexArray.push_back(Vertex(bottomleft, n, glm::vec2(U_coords[k], 0.0f)));
			//bottom right triangle
			mVertexArray.push_back(Vertex(topright, n, glm::vec2(U_coords[i], 1.0f)));
			mVertexArray.push_back(Vertex(bottomleft, n, glm::vec2(U_coords[k], 0.0f)));
			mVertexArray.push_back(Vertex(bottomright, n, glm::vec2(U_coords[i], 0.0f)));
		}
		
	}
	
	//bottom faces of the cylinder
	for (int i = 0; i < slices; i++)
	{
		//this is for final iteration
		if (i == (slices - 1))
		{
			glm::vec4 center = positions[1];
			int a = 3;
			glm::vec4 fistVert = positions[a];
			int b = 3 + (i * 2);
			glm::vec4 secondVert = positions[b];

			mVertexArray.push_back(Vertex(center, N_bottom, glm::vec2(U_coords[i], 0.0f)));
			mVertexArray.push_back(Vertex(secondVert, N_bottom, glm::vec2(U_coords[i], 0.0f)));
			mVertexArray.push_back(Vertex(fistVert, N_bottom, glm::vec2(1.0f, 0.0f)));

		}
		else
		{
			//center will be center p1
			glm::vec4 center = positions[1];
			//the higher index p5
			int a = 5 + (i*2);
			glm::vec4 fistVert = positions[a];
			//the lower index p3
			int b = 3 + (i * 2);
			glm::vec4 secondVert = positions[b];

			mVertexArray.push_back(Vertex(center, N_bottom, glm::vec2(U_coords[i], 0.0f)));
			mVertexArray.push_back(Vertex(secondVert, N_bottom, glm::vec2(U_coords[i], 0.0f)));
			int k = i + 1;
			mVertexArray.push_back(Vertex(fistVert, N_bottom, glm::vec2(U_coords[k], 0.0f)));

		}
	}
	//std::cout << "DONE MAKING CYLINDER" << std::endl;
	SetType(MeshType::CYLINDER);
	SetVertexNum(int(mVertexArray.size()));
	SetFaceNum(int(mVertexArray.size() / 3));

	ConstructAveragedNormals();
}
void Mesh::ConstructCone(int slices)
{
	std::vector<glm::vec4> positions;
	std::vector<float> U_coords;
	//center vertex, top and Bottom
	glm::vec4 p0(0.0f, 0.5f, 0.0f, 1.0f);
	glm::vec4 p1(0.0f, -0.5f, 0.0f, 1.0f);
	positions.push_back(p0);
	positions.push_back(p1);
	float angle_rad = 0.0f;
	float a = 360.0f / slices;
	float rad_increment = glm::radians(a);

	//TODO, MAKE NORMALS
	//top
	glm::vec4 N_top = glm::vec4(0.0f, 1.0f, 0.0f, 0);
	//bottom
	glm::vec4 N_bottom = glm::vec4(0.0f, -1.0f, 0.0f, 0);

	//make the positions of the cone bottom
	for (int i = 0; i < slices; i++)
	{
		//radius is == 0.5f
		float x = 0.5f * glm::cos(angle_rad);
		float z = 0.5f * glm::sin(angle_rad);
		glm::vec4 current(x, -0.5f, z, 1.0f);
		
		//std::cout << "Angle: " << angle_rad << std::endl;
		//std::cout << current.x << ", " << current.y << ", " << current.z << ", " << current.w << std::endl;
		positions.push_back(current);
		float u = angle_rad / (2 * PI);
		U_coords.push_back(u);
		angle_rad += rad_increment;
		//U coord calculations
		//TODO make PI better
		
	}
	//Do Bottom part of cone
	//p1, p2, p3
	for (int i = 0; i < slices; i++)
	{
		//this is for final iteration
		if (i == (slices - 1))
		{
			//p1, p1, p1...
			glm::vec4 center = positions[1];
			//p7
			int a = 2 + i;
			glm::vec4 p7 = positions[a];
			//p3, p4, p5...
			int b = 2;
			glm::vec4 p2 = positions[b];

			mVertexArray.push_back(Vertex(center, N_bottom, glm::vec2(U_coords[i], 0.0f)));
			mVertexArray.push_back(Vertex(p7, N_bottom, glm::vec2(U_coords[i], 0.0f)));
			mVertexArray.push_back(Vertex(p2, N_bottom, glm::vec2(1.0f, 0.0f)));
		}
		else
		{
			//p1, p1, p1...
			glm::vec4 center = positions[1];
			//p2, p3, p4...
			int a = 2 + i;
			glm::vec4 lowerNum = positions[a];
			//p3, p4, p5...
			int b = a + 1;
			glm::vec4 higherNum = positions[b];

			mVertexArray.push_back(Vertex(center, N_bottom, glm::vec2(U_coords[i], 0.0f)));
			mVertexArray.push_back(Vertex(lowerNum, N_bottom, glm::vec2(U_coords[i], 0.0f)));
			int k = i + 1;
			mVertexArray.push_back(Vertex(higherNum, N_bottom, glm::vec2(U_coords[k], 0.0f)));
		}
		
	}
	//Do sides of cone
	//p0, p2, p3/ p0, p3, p4
	for (int i = 0; i < slices; i++)
	{
		//this is for final iteration
		if (i == (slices - 1))
		{
			//p0, p0, p0...
			glm::vec4 center = positions[0];
			//p7
			int a = 2 + i;
			glm::vec4 p7 = positions[a];
			//p3, p4, p5...
			int b = 2;
			glm::vec4 p2 = positions[b];

			//make normals through cross product
			glm::vec3 v_1 = (p2 - center);
			glm::vec3 v_2 = (p7 - center);
			glm::vec3 v_3 = glm::cross(v_1, v_2);
			glm::vec3 v_4 = glm::normalize(v_3);
			glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);

			//reverse order
			mVertexArray.push_back(Vertex(center, n, glm::vec2(U_coords[i], 1.0f)));
			mVertexArray.push_back(Vertex(p2, n, glm::vec2(1.0f, 0.0f)));
			mVertexArray.push_back(Vertex(p7, n, glm::vec2(U_coords[i], 0.0f)));
		}
		else
		{
			//p0, p0, p0...
			glm::vec4 center = positions[0];
			//p2, p3, p4...
			int a = 2 + i;
			glm::vec4 lowerNum = positions[a];
			//p3, p4, p5...
			int b = a + 1;
			glm::vec4 higherNum = positions[b];

			//make normals through cross product
			glm::vec3 v_1 = (higherNum - center);
			glm::vec3 v_2 = (lowerNum - center);
			glm::vec3 v_3 = glm::cross(v_1, v_2);
			glm::vec3 v_4 = glm::normalize(v_3);
			glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);

			//p0-p3-p2, order is reversed from previous loop
			mVertexArray.push_back(Vertex(center, n, glm::vec2(U_coords[i], 1.0f)));
			int k = i + 1;
			mVertexArray.push_back(Vertex(higherNum, n, glm::vec2(U_coords[k], 0.0f)));
			mVertexArray.push_back(Vertex(lowerNum, n, glm::vec2(U_coords[i], 0.0f)));

		}

	}
	/////////////////////////////////////////////////////
	SetType(MeshType::CONE);
	SetVertexNum(int(mVertexArray.size()));
	SetFaceNum(int(mVertexArray.size() / 3));

	ConstructAveragedNormals();

}
void Mesh::ConstructSphere(int slices)
{
	int rings = slices / 2;
	std::vector<glm::vec4> positions;
	std::vector<float> U_coords;
	std::vector<float> V_coords;
	//center vertex, top and Bottom
	glm::vec4 p0(0.0f, 0.5f, 0.0f, 1.0f);
	glm::vec4 p1(0.0f, -0.5f, 0.0f, 1.0f);
	int pos_index = 0;
	positions.push_back(p0);
	pos_index += 1;
	positions.push_back(p1);
	pos_index += 1;

	//we need an angle alpha for calculating positions on the XZ plane, and an angle theta for height of rings top to bottom
	float alpha_rad = 0.0f;
	float a = 360.0f / slices;
	float alpha_increment = glm::radians(a);
	float theta_rad = 0.0f;
	float t = 180.0f / rings;
	float theta_increment = glm::radians(t);

	//TODO, MAKE NORMALS
	//top
	glm::vec4 N_top = glm::vec4(0.0f, 1.0f, 0.0f, 0);
	//bottom
	glm::vec4 N_bottom = glm::vec4(0.0f, -1.0f, 0.0f, 0);

	/************************************************************************************************************/
	//make the positions of first ring (top)
	//increment to get the correct y height later
	theta_rad += theta_increment;
	//calculate radius r' using r(0.5) and theta
	float r_prime = 0.5f * sin(theta_rad);
	float ring_height = 0.5f * cos(theta_rad);
	for (int i = 0; i < slices; i++)
	{
		//radius is == r_prime
		float x = r_prime * glm::cos(alpha_rad);
		float z = r_prime * glm::sin(alpha_rad);
		glm::vec4 current(x, ring_height, z, 1.0f);

		//std::cout << "Angle: " << alpha_rad << std::endl;
		//std::cout << current.x << ", " << current.y << ", " << current.z << ", " << current.w << std::endl;
		positions.push_back(current);
		pos_index += 1;
		//U coord calculations
		//TODO make PI better
		float u = alpha_rad / (2 * PI);
		U_coords.push_back(u);
		alpha_rad += alpha_increment;
		
	}
	//calculate the V coord values
	V_coords.push_back(0.0f);
	float v = theta_rad / (PI);
	V_coords.push_back(v);

	/************************************************************************************************************/
	
	/************************************************************************************************************/
	//make the positions of the center rings middle and also the last one
	int center_rings_num = rings - 2;
	//increment to get the correct y height later
	theta_rad += theta_increment;
	for (int i = 0; i < center_rings_num; i++)
	{
		//reset alpha angle for XZ plane calculations
		alpha_rad = 0.0f;
		//calculate ring height and r_prime for this iteration 
		ring_height = 0.5f * cos(theta_rad);
		r_prime = 0.5f * sin(theta_rad);
		for (int j = 0; j < slices; j++)
		{
			//radius is == r_prime
			float x = r_prime * glm::cos(alpha_rad);
			float z = r_prime * glm::sin(alpha_rad);
			glm::vec4 current(x, ring_height, z, 1.0f);
			//std::cout << "Angle: " << alpha_rad << std::endl;
			//std::cout << current.x << ", " << current.y << ", " << current.z << ", " << current.w << std::endl;
			positions.push_back(current);
			pos_index += 1;
			alpha_rad += alpha_increment;
		}
		//get V_coord Values
		float v = theta_rad / (PI);
		V_coords.push_back(v);
		//increment to get the correct y height later for next loop
		theta_rad += theta_increment;
	}
	//input final v coord value
	V_coords.push_back(1.0f);
	/************************************************************************************************************/
	int index_of_start_of_final_ring = pos_index - slices;
	//std::cout << index_of_start_of_final_ring << std::endl;

	/************************************************************************************************************/
	//Make vertices of top ring with p0 as top point
	for (int i = 0; i < slices; i++)
	{
		//this is for final iteration
		if (i == (slices - 1))
		{
			//p0, p0, p0...
			glm::vec4 center = positions[0];
			//p2, p3, p4...
			int a = 2 + i;
			glm::vec4 p7 = positions[a];
			//p3, p4, p5...
			int b = 2;
			glm::vec4 p2 = positions[b];

			//make normals through cross product
			glm::vec3 v_1 = (p2 - center);
			glm::vec3 v_2 = (p7 - center);
			glm::vec3 v_3 = glm::cross(v_1, v_2);
			glm::vec3 v_4 = glm::normalize(v_3);
			glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);

			//p0-p3-p2, order is reversed from previous loop
			mVertexArray.push_back(Vertex(center, n, glm::vec2(U_coords[i], 0.0f)));
			mVertexArray.push_back(Vertex(p2, n, glm::vec2(1.0f, V_coords[1])));
			mVertexArray.push_back(Vertex(p7, n, glm::vec2(U_coords[i], V_coords[1])));
		}
		else
		{
			//p0, p0, p0...
			glm::vec4 center = positions[0];
			//p2, p3, p4...
			int a = 2 + i;
			glm::vec4 lowerNum = positions[a];
			//p3, p4, p5...
			int b = a + 1;
			glm::vec4 higherNum = positions[b];

			//make normals through cross product
			glm::vec3 v_1 = (higherNum - center);
			glm::vec3 v_2 = (lowerNum - center);
			glm::vec3 v_3 = glm::cross(v_1, v_2);
			glm::vec3 v_4 = glm::normalize(v_3);
			glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);

			//p0-p3-p2, 
			mVertexArray.push_back(Vertex(center, n, glm::vec2(U_coords[i], 0.0f)));
			int k = i + 1;
			mVertexArray.push_back(Vertex(higherNum, n, glm::vec2(U_coords[k], V_coords[1])));
			mVertexArray.push_back(Vertex(lowerNum, n, glm::vec2(U_coords[i], V_coords[1])));
		}
	}
	/************************************************************************************************************/

	/************************************************************************************************************/
	//faces of the center ring triangles (we start with top left triangles)
	//index of positions for the positions array, it always starts at 2
	for (int i = 0; i < center_rings_num; i++)
	{
		for (int j = 0; j < slices; j++)
		{
			//this is for final iteration
			if (j == (slices - 1))
			{
				int a = 2 + j + (i * slices);
				glm::vec4 topright = positions[a];
				
				int b = 2 + (i * slices);
				glm::vec4 topleft = positions[b];
				
				int c = a + slices;
				glm::vec4 bottomright = positions[c];
				
				int d = b + slices;
				glm::vec4 bottomleft = positions[d];

				//make normals through cross product
				glm::vec3 v_1 = (bottomleft - topleft);
				glm::vec3 v_2 = (topright - topleft);
				glm::vec3 v_3 = glm::cross(v_1, v_2);
				glm::vec3 v_4 = glm::normalize(v_3);
				glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);


				//top left triangles
				int k = i + 1;
				int k2 = k + 1;
				mVertexArray.push_back(Vertex(topright, n, glm::vec2(U_coords[j], V_coords[k])));
				mVertexArray.push_back(Vertex(topleft, n, glm::vec2(1.0f, V_coords[k])));
				mVertexArray.push_back(Vertex(bottomleft, n, glm::vec2(1.0f, V_coords[k2])));
				//bottom right triangles
				mVertexArray.push_back(Vertex(topright, n, glm::vec2(U_coords[j], V_coords[k])));
				mVertexArray.push_back(Vertex(bottomleft, n, glm::vec2(1.0f, V_coords[k2])));
				mVertexArray.push_back(Vertex(bottomright, n, glm::vec2(U_coords[j], V_coords[k2])));
				pos_index = a;
			}
			else
			{
				int a = 2 + j + (i * slices);
				glm::vec4 topright = positions[a];

				int b = a + 1;
				glm::vec4 topleft = positions[b];

				int c = a + slices;
				glm::vec4 bottomright = positions[c];

				int d = b + slices;
				glm::vec4 bottomleft = positions[d];

				//make normals through cross product
				glm::vec3 v_1 = (bottomleft - topleft);
				glm::vec3 v_2 = (topright - topleft);
				glm::vec3 v_3 = glm::cross(v_1, v_2);
				glm::vec3 v_4 = glm::normalize(v_3);
				glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);

				//top left triangles
				int k = i + 1;
				int m = k + 1;
				int g = j + 1;
				int k2 = i + 2;
				mVertexArray.push_back(Vertex(topright, n, glm::vec2(U_coords[j], V_coords[k])));
				mVertexArray.push_back(Vertex(topleft, n, glm::vec2(U_coords[g], V_coords[k])));
				mVertexArray.push_back(Vertex(bottomleft, n, glm::vec2(U_coords[g], V_coords[k2])));
				//bottom right triangles
				mVertexArray.push_back(Vertex(topright, n, glm::vec2(U_coords[j], V_coords[k])));
				mVertexArray.push_back(Vertex(bottomleft, n, glm::vec2(U_coords[g], V_coords[k2])));
				mVertexArray.push_back(Vertex(bottomright, n, glm::vec2(U_coords[j], V_coords[k2])));

			}
		}
		
	}
	/************************************************************************************************************/
	int size_of_v = int(V_coords.size());
	/************************************************************************************************************/
	glm::vec4 DebugBottomVec(0.0f, -2.0f, 0.0f, 0.0f);
	//Make vertices of bottom slice ring with p1 as bottom point
	for (int i = 0; i < slices; i++)
	{
		//this is for final iteration
		if (i == (slices - 1))
		{
			//p1
			glm::vec4 center = positions[1];
			//p8
			int a = index_of_start_of_final_ring;
			glm::vec4 lowerNum = positions[a];
			//p13
			int b = index_of_start_of_final_ring + i;
			glm::vec4 higherNum = positions[b];

			//make normals through cross product
			glm::vec3 v_1 = (lowerNum - center);
			glm::vec3 v_2 = (higherNum - center);
			glm::vec3 v_3 = glm::cross(v_2, v_1);
			glm::vec3 v_4 = glm::normalize(v_3);
			glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);

			//p13-p8-p1
			int index1 = size_of_v - 2;
			mVertexArray.push_back(Vertex(higherNum, n, glm::vec2(U_coords[i], V_coords[index1])));
			int index2 = size_of_v - 2;
			mVertexArray.push_back(Vertex(lowerNum, n, glm::vec2(1.0f, V_coords[index2])));
			mVertexArray.push_back(Vertex(center, n, glm::vec2(U_coords[i], 1.0f)));
		}
		else
		{
			//p1, p1, p1...
			glm::vec4 center = positions[1];
			//p8, p9, p10...
			int a = index_of_start_of_final_ring + i;
			glm::vec4 lowerNum = positions[a];
			//p9, p10, p11...
			int b = a + 1;
			glm::vec4 higherNum = positions[b];

			//make normals through cross product
			glm::vec3 v_1 = (higherNum - center);
			glm::vec3 v_2 = (lowerNum - center);
			glm::vec3 v_3 = glm::cross(v_2, v_1);
			glm::vec3 v_4 = glm::normalize(v_3);
			glm::vec4 n = glm::vec4(v_4.x, v_4.y, v_4.z, 0.0f);

			//p8-p9-p1, 
			int index1 = size_of_v - 2;
			mVertexArray.push_back(Vertex(lowerNum, n, glm::vec2(U_coords[i], V_coords[index1])));
			int index2 = i + 1;
			mVertexArray.push_back(Vertex(higherNum, n, glm::vec2(U_coords[index2], V_coords[index1])));
			mVertexArray.push_back(Vertex(center, n, glm::vec2(U_coords[i], 1.0f)));
		}
	}
	/************************************************************************************************************/

	SetType(MeshType::SPHERE);
	SetVertexNum(int(mVertexArray.size()));
	SetFaceNum(int(mVertexArray.size() / 3));

	ConstructAveragedNormals();

}
void Mesh::CleanupAndReset()
{
	mVertexArray.clear();
	mNormalArray.clear();
	mAveragedNormalArray.clear();
	mVertexNum = 0;
	mFaceNum = 0;
}
void* Mesh::GetVertices()
{
	//IS THIS OK?
	auto ptr = &(*(mVertexArray.begin()));
	return (void* )(ptr);
}
void* Mesh::GetNormals()
{
	//IS THIS OK?
	auto ptr = &(*(mNormalArray.begin()));
	return (void*)(ptr);
}
void* Mesh::GetAveragedNormals()
{
	//IS THIS OK?
	auto ptr = &(*(mAveragedNormalArray.begin()));
	return (void*)(ptr);
}
int Mesh::GetVertexNum()
{
	return mVertexNum;
}
int Mesh::GetNormalsNum()
{
	return int(mNormalArray.size());
}
void Mesh::SetVertexNum(int v)
{
	mVertexNum = v;
}

int Mesh::GetFaceNum()
{
	return mFaceNum;
}
void Mesh::SetFaceNum(int f)
{
	mFaceNum = f;
}

MeshType Mesh::GetType()
{
	return mType;
}
void Mesh::SetType(MeshType t)
{
	mType = t;
}
void Mesh::GenerateNormalLines()
{
	int vertex_num = GetVertexNum();
	for (int i = 0; i < vertex_num; i++)
	{
		Vertex v = mVertexArray[i];
		glm::vec4 start = v.position;
		glm::vec4 end = v.position + v.normal;
		NormalLine n(start, end);
		mNormalArray.push_back(n);
	}
}
void Mesh::GenerateAveragedNormalLines()
{
	int vertex_num = GetVertexNum();
	for (int i = 0; i < vertex_num; i++)
	{
		Vertex v = mVertexArray[i];
		glm::vec4 start = v.position;
		glm::vec4 end = v.position + v.AveragedNormal;
		NormalLine n(start, end);
		mAveragedNormalArray.push_back(n);
	}
}

void Mesh::ConstructAveragedNormals()
{
	//create a temporary vector for the averaged normals
	std::vector<glm::vec4> averagedNormals;
	int i = 0;
	int j = 0;
	//loop through each vertex in mVertexArrays using currentVertex
	for (std::vector<Vertex>::iterator currentVertex = mVertexArray.begin(); currentVertex != mVertexArray.end(); ++currentVertex)
	{
		//std::cout << "currentVertex = vertex " << i++ << std::endl;
		//push the current vertex normal into the array
		averagedNormals.push_back(currentVertex->normal);
		//with this currentVertex  selected we loop through the vertex array again and add those vertices that
		//share the same position as currentVertex, but ignoring those that have exactly the same Normal vector value
		for (std::vector<Vertex>::iterator it = mVertexArray.begin(); it != mVertexArray.end(); ++it)
		{
			//std::cout << "it = vertex " << j << std::endl;
			//skip if its the same vertex
			if (it == currentVertex)
			{
				j++;
				continue;
			}
			//check if position is equal
			glm::vec4 var = (currentVertex)->position;
			glm::vec4 var2 = (it)->position;
			//std::cout << "currentVertex: " << var.x <<", "<< var.y << ", " << var.z << ", " << var.w << ", " << std::endl;
			//std::cout << "it           : " << var2.x <<", "<< var2.y << ", " << var2.z << ", " << var2.w << ", " << std::endl;
			if ((currentVertex)->position == (it)->position)
			{
				//Check if Normal is exactly the same, if so, skip
 				if ((currentVertex)->normal == (it)->normal)
				{
					j++;
					continue;
				}
				bool found = false;
				//iterated through the current averaged normals and check if the value of this normal is already in it
				for (std::vector<glm::vec4>::iterator temp = averagedNormals.begin(); temp != averagedNormals.end(); ++temp)
				{
					//if the normal at 'it' already exists in averaged normals, we break and skip it
					if ((*temp) == it->normal)
					{
						found = true;
						break;
					}
				}
				//if we found the normal, we skip
				if (found)
				{
					j++;
					continue;
				}
				else
				{
					//push the iterator normal into the array
					averagedNormals.push_back(it->normal);
				}
				
			}
			j++;
		}
		j = 0;
		//once you are done, average the normals
		glm::vec4 result(0.0f, 0.0f, 0.0f, 0.0f);
		for (std::vector<glm::vec4>::iterator it2 = averagedNormals.begin(); it2 != averagedNormals.end(); ++it2)
		{
			result += (*it2);
		}

		//once all of those are added to the temporary vector we average them, then set the Average normal of the current vertex to the value obtained
		//set this new normal as the averaged nomal on currentVertex
		if (glm::length(result) < 0.01f)
		{
			int k = 0;
		}
		result = glm::normalize(result);
		currentVertex->AveragedNormal = result;
		averagedNormals.clear();
	}
}
//NOTE: check if making things vec4 instead of vec3 messes things up
void Mesh::ComputeTangentBasis()
{
	// Clear and initialize tangent and bitangent containers from the vector of vertices
	for (std::vector<Vertex>::iterator it = std::begin(mVertexArray); it != std::end(mVertexArray); ++it)
	{
		it->tangents = glm::vec4(0.0f);
		it->bi_tangents = glm::vec4(0.0f);
	}
	// Loop through the triangles (All of the vertices, 3 by 3 for each triangle)
	for (int i = 0; i < mVertexArray.size(); i += 3)
	{
		//get the vectors for the CCW order of vertices of each triangle
		// unsigned short u0 = indices[i + 0];
		unsigned short u0 = i + 0;
		unsigned short u1 = i + 1;
		unsigned short u2 = i + 2;
		//0 to 1
		glm::vec4 v1 = mVertexArray[u1].position - mVertexArray[u0].position;
		glm::vec2 uv1 = mVertexArray[u1].UV - mVertexArray[u0].UV;
		//0 to 2
		glm::vec4 v2 = mVertexArray[u1].position - mVertexArray[u0].position;
		glm::vec2 uv2 = mVertexArray[u2].UV - mVertexArray[u0].UV;

		float denominator = (uv1.y * uv2.x) - (uv2.y * uv1.x);
		//calculating T and B taking into account that denominator could end up == 0
		float     invDenominator = denominator == 0.0f ? 0.0f : 1.0f / denominator;
		glm::vec4 T = (uv1.y * v2 - uv2.y * v1) * invDenominator;
		glm::vec4 B = (uv2.x * v1 - uv1.x * v2) * invDenominator;
		// Accumulate tangent/bitangent for the 3 vertices of the triangle (to average after)
		mVertexArray[u0].tangents = T;
		mVertexArray[u1].tangents = T;
		mVertexArray[u2].tangents = T;
		mVertexArray[u0].bi_tangents = B;
		mVertexArray[u1].bi_tangents = B;
		mVertexArray[u2].bi_tangents = B;

	}

}