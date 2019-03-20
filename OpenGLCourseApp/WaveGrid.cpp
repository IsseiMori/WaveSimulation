#include "WaveGrid.h"

WaveGrid::WaveGrid()
{
	timeCount = 0.0f;
}


void WaveGrid::CreateGrid(int _gridX, int _gridZ, float _gridSize, GLfloat _wavePeriod, float _waveHeight)
{
	gridX = _gridX;
	gridZ = _gridZ;
	gridSize = _gridSize;

	timeCount = 0.0f;

	iniWaveLength = 0.0f;

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

			// vertices position, uv, normal
			vertices[vHead + 0] = (GLfloat)(gridSize / gridX * x);
			vertices[vHead + 1] = 0.0f;
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

	// Initialize waveQueues vector with queues
	for (int i = 0; i < gridZ; i++)
	{
		waveQueues.push_back(std::deque<float>());
	}

	wavePeriod = _wavePeriod;
	waveHeight = _waveHeight;
}


void WaveGrid::Update(GLfloat deltaTime)
{
	UpdateWaves(deltaTime);

	UpdateVertices();

	// CalcAverageNormals(8, 5);
}


void WaveGrid::UpdateWaves(GLfloat deltaTime)
{
	timeCount += deltaTime;

	if (timeCount > wavePeriod)
	{
		timeCount = 0.0f;
	}

	// If no wave exists
	// Add a initial wave
	if (waveQueues[0].size() == 0)
	{
		for (int i = 0; i < waveQueues.size(); i++)
		{
			waveQueues[i].push_back(0.0f);
		}
	}

	// If the lastest wave gets inbounds
	// Add new outbounds wave
	if (waveQueues[0].back() >= 0.0f)
	{
		float waveLength = G * pow(wavePeriod, 2) / (2.0f * PI);

		iniWaveLength = waveLength;

		for (int i = 0; i < waveQueues.size(); i++)
		{
			waveQueues[i].push_back(-waveLength);
		}
	}


	// Update the position of each wave
	for (int z = 0; z < waveQueues.size(); z++)
	{
		float waveLength = 0.0f, celerity = 0.0f;

		for (int x = 0; x < waveQueues[z].size() - 1; x++)
		{
			waveLength = waveQueues[z][x] - waveQueues[z][x + 1];

			// Bilinear interpolation of height
			float x0 = x0z0 * (1.0f - z / (float)(gridZ)) + x0z1 * (z / (float)(gridZ));
			float x1 = x1z0 * (1.0f - z / (float)(gridZ)) + x1z1 * (z / (float)(gridZ));
			float depth = abs(x0 * (1.0f - waveQueues[z][x] / gridSize) + x1 * (waveQueues[z][x] / gridSize));

			if (depth / waveLength < 0.05f)
			{
				celerity = sqrt(G * depth);
			}
			else if(depth / waveLength < 0.5f)
			{
				celerity = sqrt( (G * waveLength) / (2.0f * PI) * tanh(2.0f * PI * depth / waveLength) );
			}
			else
			{
				celerity = sqrt(G * waveLength / (2.0f * PI));
			}

			// Move the wave
			waveQueues[z][x] += celerity * deltaTime;
		}

		// Move the outbounds wave
		waveQueues[z][waveQueues[z].size() - 1] += celerity * deltaTime;
	}

	// If the first wave is too far
	// Delete the wave
	for (int i = 0; i < waveQueues.size(); i++)
	{
		if (waveQueues[i].front() > gridSize)
		{
			waveQueues[i].pop_front();
		}
	}
}

void WaveGrid::UpdateVertices()
{
	for (int z = 0; z < gridZ; z++)
	{
		int currentWave = 0;

		for (int x = gridX - 1; x >= 0; x--)
		{
			int vHead = z * gridX * 8 + x * 8;

			float gridPosX = gridSize * x / gridX;

			// If the current grid position is in a wave
			if (waveQueues[z].size() >= 2					// There is at least one start and end of a wave
				&& gridPosX < waveQueues[z][currentWave])	// Current grid position is in a wave
			{

				// Skip until the wave the current grid position belongs
				while (gridPosX > waveQueues[z].back() && gridPosX < waveQueues[z][currentWave + 1]) 
				{
					currentWave++;
				}

				float waveLength = waveQueues[z][currentWave] - waveQueues[z][currentWave + 1];
				float crest = waveQueues[z][currentWave + 1] + waveLength / 2.0f;
				float distToCrest = abs(crest - gridPosX);

				float a = waveLength / (2.0f * PI);
				float b = waveHeight / 2.0f;

				float y = trochoidApproximation(a, b, distToCrest, 12);

				vertices[vHead + 1] = y * iniWaveLength / waveLength * 10.0f;

			}
			// The current grid position is not in a wave
			else
			{
				vertices[vHead + 1] = 0.0f;
			}

		}
	}
}


void WaveGrid::setGroundHeight(float _x0z0, float _x0z1, float _x1z0, float _x1z1)
{
	x0z0 = _x0z0;
	x0z1 = _x0z1;
	x1z0 = _x1z0;
	x1z1 = _x1z1;
}


float WaveGrid::trochoidApproximation(float a, float b, float x, int iteration)
{
	float leftVal = 0.0f;
	float rightVal = a * PI - b * sin(PI);

	float leftRad = 0.0f;
	float rightRad = PI;

	float middleRad = 0.0f;
	float middleVal = 0.0f;

	for (int i = 0; i < iteration; i++)
	{
		middleRad = (leftRad + rightRad) / 2.0f;

		middleVal = a * middleRad - b * sin(middleRad);

		if (x > middleVal)
		{
			leftRad = middleRad;
			leftVal = middleVal;
		}
		else
		{
			rightRad = middleRad;
			rightVal = middleVal;
		}
	}

	float y = a - b * cos(middleRad);

	float adjY = -(y - a);

	return adjY;
}


WaveGrid::~WaveGrid()
{
	CleanMesh();
}
