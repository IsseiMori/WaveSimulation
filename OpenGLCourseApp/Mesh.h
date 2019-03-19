#pragma once
#include <iostream>

#include <GL/glew.h>
class Mesh
{
public:

	GLfloat *vertices;
	unsigned int *indices;

	Mesh();

	void CreateMesh();

	void RenderMesh();
	void RenderMeshWithVerticesUpdated();

	void virtual Update(GLfloat deltaTime);

	void CleanMesh();

	~Mesh();
	
protected:
	unsigned int numofVertices;
	unsigned int numofIndices;

private:
	GLuint VAO, VBO, IBO;
};

