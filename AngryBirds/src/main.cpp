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

// callback de resolucion
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// procesar input
void processInput(GLFWwindow* window, float dt);

// resolucion
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camara
Camera camera(glm::vec3(0.0f, 0.0f, 50.0f));

// tiempo
double dt = 0.0f;
double lastFrame = 0.0f;

// luz punto
glm::vec3 lightPos(0.0f, 10.0f, 0.0f);

// vector de objetos
std::vector<Objeto*> pObjetos;

// instancias maestras
Esfera esfera(glm::vec3(0.0f), glm::vec3(1.0f));
Caja caja(glm::vec3(0.0f), glm::vec3(1.0f));

// instancias de uso multiple
Esfera* proyectil = new Esfera(&esfera, glm::vec3(0.0f), glm::vec3(1.0f));
float shootingAngleY = 45.0f;
float shootingAngleX = 0.0f;

Caja* dummy = new Caja(&caja, glm::vec3(0.0f), glm::vec3(1.0f));

void scene() {
	// escena de prueba

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
	// inicializacion
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// callbacks estaticos
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);
	// callback de resolucion
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// extras de openGL
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable cursor

	// shaders
	Shader lightingShader("resources/shaders/basic.vert", "resources/shaders/basic.frag");
	Shader boxShader("resources/shaders/box.vert", "resources/shaders/box.frag");

	// iniciar instancias maestras
	esfera.init();
	esfera.setup();

	caja.init();
	caja.setup();

	// preparar escena
	scene();

	// bucle principal
	while (!glfwWindowShouldClose(window)) {
		// tiempo
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		// input
		processInput(window, dt);

		// limpiar escena
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// actualizar shader
		lightingShader.use();
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.cameraPos);

		// calcular matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.getViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// renderizar objetos
		for (auto& obj : pObjetos) {
			obj->update(dt);
			// calcular si hay colision
			obj->checkCollisions(pObjetos);
			obj->display(lightingShader);
		}

		// preparar siguiente frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// limpieza


	// finalizar
	glfwTerminate();
	return 0;
}

// procesar input
void processInput(GLFWwindow* window, float dt)
{
	// cierre
	if (Keyboard::key(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	// posicion del cursor
	double dx = Mouse::getDX(), dy = Mouse::getDY();
	if (dx != 0 || dy != 0) {
		camera.updateCameraDirection(dx, dy);
	}

	// zoom
	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0) {
		camera.updateCameraZoom(scrollDy);
	}

	// mover camara
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

	// angulo para disparar esfera
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

	// disparar esfera
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

	// generar caja
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

// callback de resolucion
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}