//
// Created by hgallegos on 20/05/2022.
//

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
#include "Memory.hpp"

class BoundingVolume;
class BoundingRegion;

class Objeto {
public:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;
	std::vector<GLuint> indices;

	glm::vec3 color = glm::vec3(1.0f);

	GLuint indices_size;

	ArrayObject VAO;

	glm::mat4 model;

	BoundingVolume* bv;

	glm::vec3 size, position;
	glm::vec3 velocity, direction;
	float angle, mass;

	bool fixed = false;
	bool visible = true;

	Objeto();
	Objeto(glm::vec3 position, glm::vec3 size);

	virtual void init() = 0;
	virtual void setup() = 0;

	void display(Shader& sh);
	void calcularColision(std::vector<Objeto*> pObjetos);
	
	virtual void update(float dt) = 0;
	virtual void moverse(glm::vec3 dir) = 0;
};

class Esfera : public Objeto {
public:
	glm::vec3 center = glm::vec3(0.0f);
	float radius = 1.0f;
	int slices = 100, stacks = 100;

	Esfera(glm::vec3 position, glm::vec3 size);
	Esfera(Esfera* esf, glm::vec3 position, glm::vec3 size);

	void init();
	void setup();
	void update(float dt);
	void moverse(glm::vec3 dir);
};

class Caja : public Objeto {
public:
	glm::vec3 posmin = glm::vec3(-1.0f);
	glm::vec3 posmax = glm::vec3(1.0f);

	Caja(glm::vec3 position, glm::vec3 size);
	Caja(Caja* caja, glm::vec3 position, glm::vec3 size);

	void init();
	void setup();
	void update(float dt);
	void moverse(glm::vec3 dir);
};

#endif //LEARNOPENGL_OBJETO_H