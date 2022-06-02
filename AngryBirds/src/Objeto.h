#ifndef OBJETO_H
#define OBJETO_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath>

#include "shader_m.h"
#include "Memory.h"

class BoundingVolume;
class BoundingRegion;

class Objeto {
public:
	// vectores de posicion, normales, coordenadas, indices
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;
	std::vector<GLuint> indices;

	// color
	glm::vec3 color = glm::vec3(1.0f);

	GLuint indices_size;

	// aqui se guardan los VBOs y el EBO en el diccionario
	ArrayObject VAO;
	BufferObject EBO;
	BufferObject posVBO;
	BufferObject normalVBO;
	BufferObject texVBO;

	// para que no se caigan infinitamente
	float y_limit = 0.0f;

	// matriz de modelo
	glm::mat4 model;

	// para colisiones
	BoundingVolume* bv;

	// variables
	glm::vec3 size, position;
	glm::vec3 velocity, direction, acceleration;
	float angle, mass;
	bool fixed = false;

	// constructores
	Objeto();
	Objeto(glm::vec3 position, glm::vec3 size);

	virtual void init() = 0;
	virtual void setup() = 0;

	// renderizar
	void display(Shader& sh);
	// detectar colisiones
	void checkCollisions(std::vector<Objeto*> pObjetos);

	// actualiza la acceleracion
	void accelerate(glm::vec3 a);
	// simula un impulso
	void push(float magnitude, glm::vec3 direction);

	// actualizar instancia
	virtual void update(float dt) = 0;
};

class Esfera : public Objeto {
public:
	// exclusivo de esfera
	glm::vec3 center = glm::vec3(0.0f);
	float radius = 1.0f;
	int slices = 100, stacks = 100;

	Esfera(glm::vec3 position, glm::vec3 size);
	Esfera(Esfera* esf, glm::vec3 position, glm::vec3 size);

	void init();
	void setup();
	void update(float dt);
};

class Caja : public Objeto {
public:
	// exclusivo de caja
	glm::vec3 posmin = glm::vec3(-1.0f);
	glm::vec3 posmax = glm::vec3(1.0f);

	Caja(glm::vec3 position, glm::vec3 size);
	Caja(Caja* caja, glm::vec3 position, glm::vec3 size);

	void init();
	void setup();
	void update(float dt);
};

#endif