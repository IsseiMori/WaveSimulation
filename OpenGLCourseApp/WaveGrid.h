#pragma once
#include "Mesh.h"

class WaveGrid : public Mesh
{
public:
	WaveGrid();

	void CreateGrid(int gridN, float gridSize);

	~WaveGrid();

private:
	int gridN;
	float gridSize;
};

