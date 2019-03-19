#include "WaveGrid.h"

WaveGrid::WaveGrid()
{
	timeCount = 0.0f;
}


void WaveGrid::CreateGrid(int _gridN, float _gridSize, GLfloat _wavePeriod, float _waveHeight)
{
	gridN = _gridN;
	gridSize = _gridSize;

	timeCount = 0.0f;

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

	// Initialize waveQueues vector with queues
	for (int i = 0; i < gridN; i++)
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
}


void WaveGrid::UpdateWaves(GLfloat deltaTime)
{
	timeCount += deltaTime;

	if (timeCount > wavePeriod)
	{
		printf("add wave\n");
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

		for (int i = 0; i < waveQueues.size(); i++)
		{
			waveQueues[i].push_back(-waveLength);
		}
	}


	// Update the position of each wave
	for (int i = 0; i < waveQueues.size(); i++)
	{
		float waveLength = 0.0f, celerity = 0.0f;

		for (int j = 0; j < waveQueues[i].size() - 1; j++)
		{
			waveLength = waveQueues[i][j] - waveQueues[i][j + 1];
			celerity = sqrt(G * waveLength / (2.0f * PI));

			// Move the wave
			waveQueues[i][j] += celerity * deltaTime;
		}

		// Move the outbounds wave
		waveQueues[i][waveQueues[i].size() - 1] += celerity * deltaTime;
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
	for (int z = 0; z < gridN; z++)
	{
		int currentWave = 0;

		for (int x = gridN - 1; x >= 0; x--)
		{
			int vHead = z * gridN * 8 + x * 8;

			float gridPosX = gridSize * x / gridN;

			// If the current grid position is in a wave
			if (waveQueues[z].size() >= 2					// There is at least one start and end of a wave
				&& gridPosX < waveQueues[z][currentWave])	// Current grid position is in a wave
			{

				// Skip until the wave the current grid position belongs
				while (gridPosX > waveQueues[z].back() &&  gridPosX < waveQueues[z][currentWave + 1]) 
				{
					currentWave++;
				}

				vertices[vHead + 1] = waveQueues[z][currentWave] / 10.0f;

			}
			// The current grid position is not in a wave
			else
			{
				vertices[vHead + 1] = 0.0f;
			}

		}
	}
}


WaveGrid::~WaveGrid()
{
	CleanMesh();
}
