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
#include "WaveGrid.h"
#include "OceanGround.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Material shinnyMaterial;
Material dullMaterial;

Light mainLight;

// Vetex Shader
static const char* vShader = "shaders/shader.vert";

// Fragment Shader
static const char* fShader = "shaders/shader.frag";


void CreateObjects()
{
	float x0z0 = -100.0f, x0z1 = -200.0f, x1z0 = -5.0f, x1z1 = -1.0f;

	WaveGrid *obj1 = new WaveGrid();
	obj1->CreateGrid(100, 10, 1000.0f, 10.0f, 1.0f);
	obj1->setGroundHeight(x0z0, x0z1, x1z0, x1z1);
	obj1->CreateMesh();
	meshList.push_back(obj1);

	OceanGround *obj2 = new OceanGround();
	obj2->CreateGround(100, 10, 1000.0f, x0z0, x0z1, x1z0, x1z1);
	obj2->CreateMesh();
	meshList.push_back(obj2);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 600); // 1366, 768 or 1280, 1024 or 1024, 768
	mainWindow.Initialize();

	
	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(500.0f, 500.0f, -500.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 200.0f, 0.05f);

	shinnyMaterial = Material(0.0f, 1);

	mainLight = Light(0.27f, 0.51f, 0.7f, 1.0f, 0.0f, 1.0f, 0.3f, 0.2f);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
			uniformAmbientColor = 0, uniformAmbientIntensity = 0, uniformDirection = 0, uniformDiffuseIntensity = 0,
			uniformSpecularIntensity = 0, uniformShininess = 0;

	glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 2000.0f);

	// Record time to adjust FPS
	GLfloat deltaTime = 0.0f;
	GLfloat lastTime = 0.0f;

	bool paused = false;

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{

		// Get and handle user input events
		glfwPollEvents();

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Check if space key is pressed
		paused = mainWindow.getKeys()[GLFW_KEY_SPACE] ? true : false;

		if (!paused)
		{
			// waves.Update(deltaTime);
			meshList[0]->Update(deltaTime);


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
			uniformEyePosition = shaderList[0].GetEyePositionLocation();
			uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
			uniformShininess = shaderList[0].GetShininessLocation();

			mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColor,
				uniformDiffuseIntensity, uniformDirection);

			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

			glm::mat4 model = glm::mat4(1.0);

			// model = glm::translate(model, glm::vec3(0.0f, -100.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			
			shinnyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[0]->RenderMeshWithVerticesUpdated();

			meshList[1]->RenderMesh();

			glUseProgram(0);

			mainWindow.swapBuffers();
		}
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
