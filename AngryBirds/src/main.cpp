#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "shader_m.h"
#include "Objeto.h"
#include "BoundingVolume.h"
#include "Scene.h"
#include "Model.h"

#include "io/Keyboard.h"
#include "io/Camera.h"
#include "io/Mouse.h"

#include <iostream>

// procesar input
void processInput(float dt);

// escena
Scene scene;

// tiempo
double dt = 0.0f;
double lastFrame = 0.0f;

glm::vec3 lightPos(0.0f, 10.0f, 0.0f);

int main() {
	// inicializar escenario
	scene = Scene(3, 3, "OpenGL", 800, 600);
	if (!scene.init()) {
		std::cout << "Error al abrir ventana" << std::endl;
		glfwTerminate();
		return -1;
	}

	// inicializar camara
	scene.camera = new Camera(glm::vec3(0.0f, 0.0f, 50.0f));

	// shaders
	//Shader lightingShader("resources/shaders/basic.vert", "resources/shaders/basic.frag");
	Shader modelShader("resources/shaders/model.vert", "resources/shaders/model.frag");

	Model skull("resources/models/skull/skull.obj");

	//scene.addObject(medical);

	// bucle principal
	while (!scene.shouldClose()) {
		// tiempo
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		// input
		processInput(dt);

		// limpiar ventana
		scene.update();

		modelShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(scene.camera->getZoom()), (float)Scene::scrWidth / (float)Scene::scrHeight, 0.1f, 100.0f);
		glm::mat4 view = scene.camera->getViewMatrix();
		modelShader.setMat4("projection", projection);
		modelShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelShader.setMat4("model", model);
		skull.Draw(modelShader);

		// preparar cuadro
		scene.newFrame();
	}

	// limpieza

	scene.cleanup();

	return 0;
}


// procesar input
void processInput(float dt)
{
	// cierre de ventana
	if (Keyboard::keyWentDown(GLFW_KEY_ESCAPE)) {
		scene.setShouldClose(true);
	}

	// input general (camara)
	scene.processInput(dt);
}