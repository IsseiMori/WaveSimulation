#include "OceanGround.h"

OceanGround::OceanGround()
{
}


void OceanGround::CreateGround(int _gridX, int _gridZ, float _gridSize, float x0z0, float x0z1, float x1z0, float x1z1)
{
	gridX = _gridX;
	gridZ = _gridZ;
	gridSize = _gridSize;


	/*
	  x
	  ***(0,0)
	  ***
	  ***z
	*/

	numofVertices = gridX * gridZ * 8;
	numofIndices = (gridX - 1) * (gridZ - 1) * 6;

	vertices = new GLfloat[numofVertices];
	indices = new unsigned int[numofIndices];

	for (int z = 0; z < gridZ; z++)
	{
		for (int x = 0; x < gridX; x++)
		{
			// Define the location of the first entry
			int vHead = z * gridX * 8 + x * 8;

			// Bilinear interpolation of height
			float x0 = x0z0 * (1.0f - z / (float)(gridZ)) + x0z1 * (z / (float)(gridZ));
			float x1 = x1z0 * (1.0f - z / (float)(gridZ)) + x1z1 * (z / (float)(gridZ));
			float xz = x0 * (1.0f - x / (float)(gridX)) + x1 * (x / (float)(gridX));

			// vertices position, uv, normal
			vertices[vHead + 0] = (GLfloat)(gridSize / gridX * x);
			vertices[vHead + 1] = xz;
			vertices[vHead + 2] = (GLfloat)(gridSize / gridZ * z);
			vertices[vHead + 3] = (GLfloat)(1.0f / gridX * x);
			vertices[vHead + 4] = (GLfloat)(1.0f / gridZ * z);
			vertices[vHead + 5] = 0.0f;
			vertices[vHead + 6] = 1.0f;
			vertices[vHead + 7] = 0.0f;

			if (z != gridZ - 1 && x != gridX - 1)
			{
				int iHead = z * (gridX - 1) * 6 + x * 6;
				int corner = x + gridX * z;
				indices[iHead + 0] = corner;
				indices[iHead + 1] = corner + 1;
				indices[iHead + 2] = corner + gridX + 1;
				indices[iHead + 3] = corner;
				indices[iHead + 4] = corner + gridX + 1;
				indices[iHead + 5] = corner + gridX;
			}

		}
	}
}


OceanGround::~OceanGround()
{
}
