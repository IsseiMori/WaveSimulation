#pragma once

#include <vector>
#include <deque>
#include <cmath>

#include "Mesh.h"

class WaveGrid : public Mesh
{
public:
	WaveGrid();

	void CreateGrid(int gridN, float gridSize, GLfloat _wavePeriod, float _waveHeight);

	void Update(GLfloat deltaTime);

	void UpdateWaves(GLfloat deltaTime);

	void UpdateVertices();

	~WaveGrid();

private:

	const float PI = 3.14f;
	const float G = 9.81f;

	int gridN;
	float gridSize;

	std::vector<std::deque<float>> waveQueues;

	GLfloat wavePeriod;

	float waveHeight;

	GLfloat timeCount;
};
