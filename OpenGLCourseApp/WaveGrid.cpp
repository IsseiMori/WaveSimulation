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
	printf("%d\n", waveQueues[0].size());
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

				vertices[vHead + 1] = abs(crest - gridPosX);

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
