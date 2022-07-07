
#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"

#include "Objeto.h"
#include "shader_m.h"

class Scene {
public:
	static unsigned int scrWidth;
	static unsigned int scrHeight;

	// objetos y camara
	std::vector<Objeto*> objects;
	Camera* camera;

	// callback de resolucion
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	// constructores
	Scene() {};
	Scene(int versionMajor, int versionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight);

	// inicializar
	bool init();

	// procesar input
	void processInput(float dt);

	// limpiar ventana
	void update();

	// preparar cuadro
	void newFrame();

	// agregar objeto a la escena
	void addObject(Objeto* obj);

	// preparar shader
	void renderShader(Shader& shader, glm::vec3 ligtPos);

	// preparar shader de normales
	void renderNormalShader(Shader& shader, glm::vec3 lightPos);

	// render sin collision
	void renderOne(Shader& shader, float dt, Objeto* obj);

	// especial
	void renderQuad(Shader& shader, float dt, Quad* quad);

	// renderizar objetos
	void render(Shader& shader, float dt);

	// limpieza
	void cleanup();

	// abstraen el cierre
	bool shouldClose();
	void setShouldClose(bool shouldClose);

	// matrices
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cameraPos;

protected:
	GLFWwindow* window;

	const char* title;

	int versionMinor;
	int versionMajor;
};

#endif