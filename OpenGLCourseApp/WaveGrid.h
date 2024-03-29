#pragma once

#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>

#include "Mesh.h"

class WaveGrid : public Mesh
{
public:
	WaveGrid();

	void CreateGrid(int _gridX, int _gridZ, float _gridSizeX, float _gridSizeZ, GLfloat _wavePeriod, float _waveHeight);

	void Update(GLfloat deltaTime);

	void UpdateWaves(GLfloat deltaTime);

	void UpdateVertices();

	void setGroundHeight(float _x0z0, float _x0z1, float _x1z0, float _x1z1);

	~WaveGrid();

private:

	const float PI = 3.14f;
	const float G = 9.81f;

	int gridX, gridZ;
	float gridSizeX, gridSizeZ;

	std::vector<std::deque<float>> waveQueues;

	GLfloat wavePeriod;

	float waveHeight;

	float iniWaveLength;

	GLfloat timeCount;

	float x0z0, x0z1, x1z0, x1z1;

	float trochoidApproximation(float a, float b, float x, int iteration);
};

