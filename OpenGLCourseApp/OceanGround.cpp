#include "OceanGround.h"

OceanGround::OceanGround()
{
}


void OceanGround::CreateGround(int _gridN, float _gridSize, float x0z0, float x0z1, float x1z0, float x1z1)
{
	gridN = _gridN;
	gridSize = _gridSize;


	/*
	  x
	  ***(0,0)
	  ***
	  ***z
	*/

	numofVertices = gridN * gridN * 8;
	numofIndices = (gridN - 1) * (gridN - 1) * 6;

	vertices = new GLfloat[numofVertices];
	indices = new unsigned int[numofIndices];

	for (int z = 0; z < gridN; z++)
	{
		for (int x = 0; x < gridN; x++)
		{
			// Define the location of the first entry
			int vHead = z * gridN * 8 + x * 8;

			// Bilinear interpolation of height
			float x0 = x0z0 * (1.0f - z / (float)(gridN)) + x0z1 * (z / (float)(gridN));
			float x1 = x1z0 * (1.0f - z / (float)(gridN)) + x1z1 * (z / (float)(gridN));
			float xz = x0 * (1.0f - x / (float)(gridN)) + x1 * (x / (float)(gridN));

			// vertices position, uv, normal
			vertices[vHead + 0] = (GLfloat)(gridSize / gridN * x);
			vertices[vHead + 1] = xz;
			vertices[vHead + 2] = (GLfloat)(gridSize / gridN * z);
			vertices[vHead + 3] = (GLfloat)(1.0f / gridN * x);
			vertices[vHead + 4] = (GLfloat)(1.0f / gridN * z);
			vertices[vHead + 5] = 0.0f;
			vertices[vHead + 6] = 1.0f;
			vertices[vHead + 7] = 0.0f;

			if (z != gridN - 1 && x != gridN - 1)
			{
				int iHead = z * (gridN - 1) * 6 + x * 6;
				int corner = x + gridN * z;
				indices[iHead + 0] = corner;
				indices[iHead + 1] = corner + 1;
				indices[iHead + 2] = corner + gridN + 1;
				indices[iHead + 3] = corner;
				indices[iHead + 4] = corner + gridN + 1;
				indices[iHead + 5] = corner + gridN;
			}

		}
	}
}


OceanGround::~OceanGround()
{
}
