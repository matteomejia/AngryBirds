#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "Objeto.h"
#include "BoundingVolume.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 50.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float tiempoInicial = 0.0f, tiempoTranscurrido = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
GLuint luna_vao;
int luna_numIndices;
GLint POSITION_ATTRIBUTE = 0, NORMAL_ATTRIBUTE = 1, TEXCOORD0_ATTRIBUTE = 8;

std::vector<Objeto*> pObjetos;
Esfera esfera(glm::vec3(0), 2., 100, 100);
bool proyectil_listo = false;
Esfera* proyectil = new Esfera();


Caja caja(glm::vec3(0.0f), glm::vec3(1.0f));

void Escena1() {
	Esfera* esfera1 = new Esfera(glm::vec3(30, 0, 0));
	esfera1->VAO = esfera.VAO;
	esfera1->indices_size = esfera.indices_size;
	esfera1->radius = esfera.radius;
	esfera1->fijo = true;
	esfera1->bv = new BoundingBox();
	esfera1->bv->calcular(*esfera1);
	pObjetos.emplace_back(esfera1);

	Esfera* esfera2 = new Esfera(glm::vec3(40, 0, 0));
	esfera2->VAO = esfera.VAO;
	esfera2->indices_size = esfera.indices_size;
	esfera2->radius = esfera.radius;
	esfera2->fijo = true;
	esfera2->bv = new BoundingBox();
	esfera2->bv->calcular(*esfera2);
	pObjetos.emplace_back(esfera2);

	Caja* caja1 = new Caja(caja.posmin, caja.posmax);
	caja1->VAO = caja.VAO;
	caja1->indices_size = caja.indices_size;
	caja1->fijo = true;
	caja1->bv = new BoundingBox();
	caja1->bv->calcular(*caja1);
	pObjetos.emplace_back(caja1);
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("resources/shaders/basic.vert", "resources/shaders/basic.frag");
	//Shader lightCubeShader("../2.2.light_cube.vs", "../2.2.light_cube.fs");

	esfera.init();
	esfera.setup();

	caja.init();
	caja.setup();

	Escena1();


	// render loop
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		tiempoTranscurrido = currentFrame - tiempoInicial; //static_cast<float>(glfwGetTime());
		std::cout << tiempoInicial << "\t";
		processInput(window);
		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		for (auto& obj : pObjetos) {
			if (!obj->fijo) {
				obj->actualizarDatos(tiempoTranscurrido);
			}
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
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		if (!proyectil_listo) {
			float x = rand() % 10;
			float y = rand() % 10;
			float z = rand() % 10;
			proyectil->VAO = esfera.VAO;
			proyectil->indices_size = esfera.indices_size;
			proyectil->radius = esfera.radius;
			proyectil->bv = new BoundingBox();
			proyectil->bv->calcular(*proyectil);
			pObjetos.emplace_back(proyectil);
			proyectil_listo = true;
			proyectil->vel_ini = glm::vec3(20, 10, 0);
			proyectil->pos_ini = glm::vec3(x, y, z);
			proyectil->ang_ini = 45;
			tiempoInicial = static_cast<float>(glfwGetTime());
		}
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
		proyectil_listo = false;


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


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}