#include "WaveGrid.h"

WaveGrid::WaveGrid()
{
}


void WaveGrid::CreateGrid(int _gridN, float _gridSize)
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

			// vertices position, uv, normal
			vertices[vHead + 0] = (GLfloat)(gridSize / gridN * x);
			vertices[vHead + 1] = 0.0f;
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


WaveGrid::~WaveGrid()
{
	CleanMesh();
}
