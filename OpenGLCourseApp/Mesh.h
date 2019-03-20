#pragma once
#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>

class Mesh
{
public:

	GLfloat *vertices;
	unsigned int *indices;

	Mesh();

	void CreateMesh();

	void CreateCube();

	void RenderMesh();
	void RenderMeshWithVerticesUpdated();

	void virtual Update(GLfloat deltaTime);

	void CalcAverageNormals(unsigned int vLength, unsigned int normalOffset);

	void CleanMesh();

	~Mesh();
	
protected:
	unsigned int numofVertices;
	unsigned int numofIndices;

private:
	GLuint VAO, VBO, IBO;
};

