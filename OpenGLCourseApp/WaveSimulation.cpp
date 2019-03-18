#define STB_IMAGE_IMPLEMENTATION

#include "pch.h"
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

const float toRadians = 3.14159265f / 180.0f;

const float gridSize = 10;
const int gridN = 5;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Light mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vetex Shader
static const char* vShader = "shaders/shader.vert";

// Fragment Shader
static const char* fShader = "shaders/shader.frag";

void calcAverageNormals(unsigned int * indices, unsigned int indicesCount, GLfloat * vertices, unsigned int verticesCount, unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indicesCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		
		in0 += normalOffset; in1 += normalOffset, in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticesCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	/*
	  x
	  ***(0,0)
	  ***
	  ***z
	*/
	GLfloat vertices[gridN * gridN * 8];
	unsigned int indices[(gridN - 1) * (gridN - 1) * 6];
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

	// calcAverageNormals(indices, (gridN - 1) * (gridN - 1) * 6, vertices, gridN * gridN * 8, 8, 5);

	for (int i = 0; i < gridN * gridN * 8; i++)
	{
		if (i % 8 == 0) printf("\n");
		printf("%f ", vertices[i]);
	}

	for (int i = 0; i < (gridN - 1) * (gridN - 1) * 6; i++)
	{
		if (i % 6 == 0) printf("\n");
		printf("%d ", indices[i]);
	}

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, gridN * gridN * 8, (gridN - 1) * (gridN - 1) * 6);
	meshList.push_back(obj1);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialize();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 2.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.05f);

	mainLight = Light(1.0f, 1.0f, 1.0f, 0.2f, 2.0f, 3.0f, -2.0f, 1.0f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, 
			uniformAmbientColor = 0, uniformAmbientIntensity = 0, uniformDirection = 0, uniformDiffuseIntensity = 0;

	glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get and handle user input events
		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformAmbientColor = shaderList[0].GetAmbientColorLocation();
		uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
		uniformDirection = shaderList[0].GetDirectionLocation();
		uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();

		mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColor, 
							uniformDiffuseIntensity, uniformDirection);
			
		glm::mat4 model = glm::mat4(1.0);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		meshList[0]->RenderMesh();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}
	
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
