#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "Objeto.h"
#include "BoundingVolume.h"
#include "Scene.h"

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

void generateScene(Scene& scene) {
	// escena de prueba

	Caja* plane = new Caja(&caja, glm::vec3(0.0f, -2.1f, 0.0f), glm::vec3(100.0f, 1.0f, 100.0f));
	plane->y_limit = -2.1f;
	plane->fixed = true;
	plane->bv->transform(plane);
	plane->color = glm::vec3(0.6f, 0.6f, 0.6f);
	scene.addObject(plane);

	Caja* caja1 = new Caja(&caja, glm::vec3(30.0f, 5.0f, 0.0f), glm::vec3(1.0f));
	caja1->fixed = true;
	caja1->bv->transform(caja1);
	caja1->color = glm::vec3(0.0f, 1.0f, 0.0f);
	scene.addObject(caja1);

	Caja* caja2 = new Caja(&caja, glm::vec3(30.0f, 5.0f, 5.0f), glm::vec3(1.0f));
	caja2->fixed = true;
	caja2->bv->transform(caja2);
	caja2->color = glm::vec3(0.0f, 0.0f, 1.0f);
	scene.addObject(caja2);

	Caja* caja3 = new Caja(&caja, glm::vec3(30.0f, 5.0f, -5.0f), glm::vec3(1.0f));
	caja3->fixed = true;
	caja3->bv->transform(caja3);
	caja3->color = glm::vec3(1.0f, 1.0f, 0.0f);
	scene.addObject(caja3);
}

int main() {
	// inicializar escenario
	scene = Scene(3, 3, "Proyecto 2", 800, 600);
	if (!scene.init()) {
		std::cout << "Error al abrir ventana" << std::endl;
		glfwTerminate();
		return -1;
	}

	// inicializar camara
	scene.camera = new Camera(glm::vec3(0.0f, 0.0f, 50.0f));

	// shaders
	Shader lightingShader("resources/shaders/basic.vert", "resources/shaders/basic.frag");

	// iniciar instancias maestras
	esfera.init();
	esfera.setup();

	caja.init();
	caja.setup();

	// preparar escena
	generateScene(scene);

	// bucle principal
	while (!scene.shouldClose()) {
		// tiempo
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		// limpiar ventana
		scene.update();

		// input
		processInput(dt);

		// actualizar shader
		scene.renderShader(lightingShader, lightPos);

		// renderizar objetos
		scene.render(lightingShader, dt);

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
		scene.addObject(proyectil);

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
		scene.addObject(dummy);

	}
}