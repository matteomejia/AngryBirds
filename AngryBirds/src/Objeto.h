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
#include "RigidBody.h"

class BoundingVolume;
class BoundingRegion;

class Objeto {
public:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;
	std::vector<GLuint> indices;

	//std::vector<BoundingRegion*> regions;

	GLuint indices_size;
	GLuint vao;

	ArrayObject VAO;

	glm::mat4 model;
	bool visible = true;
	BoundingVolume* bv;

	glm::vec3 vel_ini, pos_ini;
	float ang_ini;
	bool fijo = false;
	glm::vec3 dir;

	void addInstance(glm::vec3 size, float mass, glm::vec3 pos);

	virtual void init() = 0;
	virtual GLuint setup() = 0;
	virtual void display(Shader& sh) = 0;
	virtual void actualizarDatos(float t) = 0;
	virtual void calcularColision(std::vector<Objeto*> pObjetos) = 0;
	virtual void moverse(glm::vec3 dir) = 0;
};

class Esfera :public Objeto {
public:
	glm::vec3 centro;
	float radius;
	int slices, stacks;

	Esfera() {
		centro = glm::vec3(0.0);
		pos_ini = centro;
		vel_ini = glm::vec3(0);
	}
	Esfera(glm::vec3 _centro) {
		centro = _centro;
		pos_ini = centro;
		vel_ini = glm::vec3(0);
	}
	Esfera(glm::vec3 _centro, float _radius, int _slices, int _stacks) {
		centro = _centro;
		pos_ini = centro;
		vel_ini = glm::vec3(0);
		radius = _radius;
		slices = _slices;
		stacks = _stacks;
	}

	void init();
	GLuint setup();
	void display(Shader& sh);
	void actualizarDatos(float t);
	void calcularColision(std::vector<Objeto*> pObjetos);
	void moverse(glm::vec3 dir);
};

class Caja : public Objeto {
public:
	glm::vec3 posmin, posmax;

	Caja(glm::vec3 min, glm::vec3 max) :posmin(min), posmax(max) {
		indices_size = 36;
	}
	Caja() {
		indices_size = 36;
		posmin = glm::vec3(0.0);
		posmax = glm::vec3(1.0);
	}

	void init();
	GLuint setup();
	void display(Shader& sh);
	void actualizarDatos(float t);
	void calcularColision(std::vector<Objeto*> pObjetos);
	void moverse(glm::vec3 dir);
};

#endif //LEARNOPENGL_OBJETO_H