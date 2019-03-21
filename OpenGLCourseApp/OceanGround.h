#pragma once
#include "Mesh.h"

class OceanGround :
	public Mesh
{
public:
	OceanGround();

	void CreateGround(int _gridX, int _gridZ, float _gridSizeX, float _gridSizeZ, float x0z0, float x0z1, float x1z0, float x1z1);

	~OceanGround();

private:
	int gridX, gridZ;
	float gridSizeX, gridSizeZ;
};

