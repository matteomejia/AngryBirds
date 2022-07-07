#include "Scene.h"

unsigned int Scene::scrWidth = 0;
unsigned int Scene::scrHeight = 0;

// callback estatico pq hay uno solo
void Scene::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	Scene::scrWidth = width;
	Scene::scrHeight = height;
}

// constructor
Scene::Scene(int versionMajor, int versionMinor,
	const char* title, unsigned int scrWidth, unsigned int scrHeight)
	: versionMajor(versionMajor), versionMinor(versionMinor), title(title) {

	Scene::scrWidth = scrWidth;
	Scene::scrHeight = scrHeight;
}

// inicializacion
bool Scene::init() {
	// inicializar GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(scrWidth, scrHeight, title, NULL, NULL);
	if (window == NULL) {
		return false;
	}
	glfwMakeContextCurrent(window);

	// inicializar GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}

	// callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	// extras
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// exito
	return true;
}

// procesar input
void Scene::processInput(float dt) {

	// direccion de la camara
	double dx = Mouse::getDX(), dy = Mouse::getDY();
	if (dx != 0 || dy != 0) {
		camera->updateCameraDirection(dx, dy);
	}

	// zoom
	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0) {
		camera->updateCameraZoom(scrollDy);
	}

	// posicion de la camara
	if (Keyboard::key(GLFW_KEY_W)) {
		camera->updateCameraPos(CameraDirection::FORWARD, dt * 5);
	}
	if (Keyboard::key(GLFW_KEY_S)) {
		camera->updateCameraPos(CameraDirection::BACKWARD, dt * 5);
	}
	if (Keyboard::key(GLFW_KEY_D)) {
		camera->updateCameraPos(CameraDirection::RIGHT, dt * 5);
	}
	if (Keyboard::key(GLFW_KEY_A)) {
		camera->updateCameraPos(CameraDirection::LEFT, dt * 5);
	}
	if (Keyboard::key(GLFW_KEY_SPACE)) {
		camera->updateCameraPos(CameraDirection::UP, dt * 5);
	}
	if (Keyboard::key(GLFW_KEY_LEFT_CONTROL)) {
		camera->updateCameraPos(CameraDirection::DOWN, dt * 5);
	}

	// matrices
	view = camera->getViewMatrix();
	projection = glm::perspective(
		glm::radians(camera->getZoom()),
		(float)scrWidth / (float)scrHeight,
		0.1f, 100.0f
	);

	cameraPos = camera->cameraPos;

}

// limpiar ventana
void Scene::update() {
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// preparar cuadro
void Scene::newFrame() {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

// agregar objeto
void Scene::addObject(Objeto* obj) {
	objects.emplace_back(obj);
}

// preparar shader
void Scene::renderShader(Shader& shader, glm::vec3 lightPos) {
	shader.use();
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader.setVec3("lightPos", lightPos);
	shader.setVec3("viewPos", camera->cameraPos);

	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
}

void Scene::renderNormalShader(Shader& shader, glm::vec3 lightPos) {
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	// normal-mapped quad
	shader.setVec3("viewPos", camera->cameraPos);
	shader.setVec3("lightPos", lightPos);
}

// renderizar objetos
void Scene::render(Shader& shader, float dt) {
	for (auto& obj : objects) {
		obj->update(dt);
		obj->checkCollisions(objects);
		obj->display(shader);
	}
}

void Scene::renderOne(Shader& shader, float dt, Objeto* obj) {
	//obj->update(dt);
	obj->display(shader);
}

void Scene::renderQuad(Shader& shader, float dt, Quad* quad) {
	//obj->update(dt);
	quad->display(shader);
}

// cerrar ventana
void Scene::cleanup() {
	glfwTerminate();
}

// estado del cierre
bool Scene::shouldClose() {
	return glfwWindowShouldClose(window);
}

// cerrar ventana
void Scene::setShouldClose(bool shouldClose) {
	glfwSetWindowShouldClose(window, shouldClose);
}