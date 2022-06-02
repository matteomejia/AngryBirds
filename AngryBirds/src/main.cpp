#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "Objeto.h"
#include "BoundingVolume.h"

#include "io/Keyboard.h"
#include "io/Camera.h"
#include "io/Mouse.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float dt);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 50.0f));

// timing
double dt = 0.0f; // tme btwn frames
double lastFrame = 0.0f; // time of last frame

// lighting
glm::vec3 lightPos(0.0f, 10.0f, 0.0f);
GLuint luna_vao;
int luna_numIndices;

std::vector<Objeto*> pObjetos;

Esfera esfera(glm::vec3(0.0f), glm::vec3(1.0f));
Caja caja(glm::vec3(0.0f), glm::vec3(1.0f));

Esfera* proyectil = new Esfera(&esfera, glm::vec3(0.0f), glm::vec3(1.0f));
float shootingAngleY = 45.0f;
float shootingAngleX = 0.0f;

Caja* dummy = new Caja(&caja, glm::vec3(0.0f), glm::vec3(1.0f));

void Escena1() {
	Caja* plane = new Caja(&caja, glm::vec3(0.0f, -2.1f, 0.0f), glm::vec3(100.0f, 1.0f, 100.0f));
	plane->y_limit = -2.1f;
	plane->fixed = true;
	plane->bv->transform(plane);
	plane->color = glm::vec3(0.6f, 0.6f, 0.6f);
	pObjetos.emplace_back(plane);

	Caja* caja1 = new Caja(&caja, glm::vec3(30.0f, 5.0f, 0.0f), glm::vec3(1.0f));
	caja1->fixed = true;
	caja1->bv->transform(caja1);
	caja1->color = glm::vec3(0.0f, 1.0f, 0.0f);
	pObjetos.emplace_back(caja1);

	Caja* caja2 = new Caja(&caja, glm::vec3(30.0f, 5.0f, 5.0f), glm::vec3(1.0f));
	caja2->fixed = true;
	caja2->bv->transform(caja2);
	caja2->color = glm::vec3(0.0f, 0.0f, 1.0f);
	pObjetos.emplace_back(caja2);

	Caja* caja3 = new Caja(&caja, glm::vec3(30.0f, 5.0f, -5.0f), glm::vec3(1.0f));
	caja3->fixed = true;
	caja3->bv->transform(caja3);
	caja3->color = glm::vec3(1.0f, 1.0f, 0.0f);
	pObjetos.emplace_back(caja3);
}

int main() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable cursor

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("resources/shaders/basic.vert", "resources/shaders/basic.frag");
	Shader boxShader("resources/shaders/box.vert", "resources/shaders/box.frag");
	//Shader lightCubeShader("../2.2.light_cube.vs", "../2.2.light_cube.fs");

	esfera.init();
	esfera.setup();

	caja.init();
	caja.setup();

	Escena1();


	// render loop
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		processInput(window, dt);

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		//lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.cameraPos);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.getViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		for (auto& obj : pObjetos) {
			obj->update(dt);
			// calcular si hay colision
			obj->calcularColision(pObjetos);
			obj->display(lightingShader);
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, luna_vao);
	//glDeleteVertexArrays(1, &lightCubeVAO);
	//glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, float dt)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	double dx = Mouse::getDX(), dy = Mouse::getDY();
	if (dx != 0 || dy != 0) {
		camera.updateCameraDirection(dx, dy);
	}

	// set camera zoom
	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0) {
		camera.updateCameraZoom(scrollDy);
	}

	if (Keyboard::key(GLFW_KEY_W))
		camera.updateCameraPos(CameraDirection::FORWARD, dt);

	if (Keyboard::key(GLFW_KEY_S))
		camera.updateCameraPos(CameraDirection::BACKWARD, dt);

	if (Keyboard::key(GLFW_KEY_A))
		camera.updateCameraPos(CameraDirection::LEFT, dt);

	if (Keyboard::key(GLFW_KEY_D))
		camera.updateCameraPos(CameraDirection::RIGHT, dt);

	if (Keyboard::key(GLFW_KEY_SPACE))
		camera.updateCameraPos(CameraDirection::UP, dt);

	if (Keyboard::key(GLFW_KEY_LEFT_CONTROL))
		camera.updateCameraPos(CameraDirection::DOWN, dt);

	if (Keyboard::keyWentDown(GLFW_KEY_UP)) {
		shootingAngleY += 5.0f;
		if (shootingAngleY > 90.0f) {
			shootingAngleY = 90.0f;
		}
		std::cout << "Angulo Y: " << shootingAngleY << std::endl;
	}

	if (Keyboard::keyWentDown(GLFW_KEY_DOWN)) {
		shootingAngleY -= 5.0f;
		if (shootingAngleY < 0.0f) {
			shootingAngleY = 0.0f;
		}
		std::cout << "Angulo Y: " << shootingAngleY << std::endl;
	}

	if (Keyboard::keyWentDown(GLFW_KEY_RIGHT)) {
		shootingAngleX += 5.0f;
		if (shootingAngleX > 45.0f) {
			shootingAngleX = 45.0f;
		}
		std::cout << "Angulo X: " << shootingAngleX << std::endl;
	}

	if (Keyboard::keyWentDown(GLFW_KEY_LEFT)) {
		shootingAngleX -= 5.0f;
		if (shootingAngleX < -45.0f) {
			shootingAngleX = -45.0f;
		}
		std::cout << "Angulo X: " << shootingAngleX << std::endl;
	}

	if (Keyboard::keyWentDown(GLFW_KEY_E)) {
		proyectil = new Esfera(&esfera, glm::vec3(0.0f), glm::vec3(1.0f));
		proyectil->push(100.0f, glm::vec3(
			cos(glm::radians(shootingAngleY)),
			sin(glm::radians(shootingAngleY)),
			sin(glm::radians(shootingAngleX))
		));
		proyectil->accelerate(glm::vec3(0.0f, -9.81f, 0.0f));

		proyectil->color = glm::vec3(1.0f, 0.1f, 0.1f);
		proyectil->indices_size = esfera.indices_size;
		proyectil->bv->transform(proyectil);
		pObjetos.emplace_back(proyectil);

	}

	if (Keyboard::keyWentDown(GLFW_KEY_G)) {
		float z = (-20.0f) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (20.0f - (-20.0f))));
		dummy = new Caja(&caja, glm::vec3(30.0f, 0.0f, z), glm::vec3(1.0f));
		dummy->accelerate(glm::vec3(0.0f, -9.81f, 0.0f));

		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		dummy->color = glm::vec3(r, g, b);
		dummy->indices_size = caja.indices_size;
		dummy->bv->transform(dummy);
		pObjetos.emplace_back(dummy);

	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}