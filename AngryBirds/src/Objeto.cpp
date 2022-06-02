//
// Created by hgallegos on 20/05/2022.
//

#include "Objeto.h"
#include "BoundingVolume.h"

Objeto::Objeto()
	: position(glm::vec3(0.0f)), size(glm::vec3(1.0f)), velocity(glm::vec3(0.0f)), direction(glm::vec3(0.0f)) {}

Objeto::Objeto(glm::vec3 position, glm::vec3 size)
	: position(position), size(size), velocity(glm::vec3(0.0f)), direction(glm::vec3(0.0f)) {}

void Objeto::display(Shader& sh) {
	model = glm::mat4(1.0f);
	model = glm::scale(model, size);
	model = glm::translate(model, position);
	sh.setMat4("model", model);
	sh.setVec3("objectColor", color);
	if (visible) {
		VAO.bind();
		VAO.draw(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, indices.data());
		ArrayObject::clear();
	}
}

void Objeto::calcularColision(std::vector<Objeto*> pObjetos) {
	for (Objeto* obj : pObjetos) {
		if (obj != this and bv->checkCollision(obj->bv)) {

			std::cout << "Positions" << std::endl;
			std::cout << glm::to_string(position) << " - " << glm::to_string(obj->position) << std::endl;

			std::cout << "Min & Max" << std::endl;
			std::cout << glm::to_string(bv->min) << ":" << glm::to_string(bv->max) << std::endl;
			std::cout << glm::to_string(obj->bv->min) << ":" << glm::to_string(obj->bv->max) << std::endl;
			obj->moverse(direction);
		}
	}
}

Esfera::Esfera(glm::vec3 position, glm::vec3 size)
	: Objeto::Objeto(position, size) {}

Esfera::Esfera(Esfera* esf, glm::vec3 position, glm::vec3 size) : Objeto::Objeto(position, size) {
	VAO = esf->VAO;
	indices_size = esf->indices_size;
	bv = new BoundingVolume(center, radius);
	bv->transform(this);
}

void Esfera::init() {
	const float _2pi = 2.0f * M_PI;

	for (int i = 0; i <= stacks; ++i)
	{
		// V texture coordinate.
		float V = i / (float)stacks;
		float phi = V * M_PI;
		for (int j = 0; j <= slices; ++j)
		{
			// U texture coordinate.
			float U = j / (float)slices;
			float theta = U * _2pi;
			float X = cos(theta) * sin(phi);
			float Y = cos(phi);
			float Z = sin(theta) * sin(phi);
			positions.push_back(glm::vec3(X, Y, Z) * radius);
			normals.push_back(glm::vec3(X, Y, Z));
			textureCoords.push_back(glm::vec2(U, V));
		}
	}
	// Now generate the index buffer
	//vector<GLuint> indicies;
	for (int i = 0; i < slices * stacks + slices; ++i) {
		indices.push_back(i);
		indices.push_back(i + slices + 1);
		indices.push_back(i + slices);
		indices.push_back(i + slices + 1);
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	indices_size = indices.size();
}

void Esfera::setup() {
	VAO = ArrayObject();

	VAO.generate();
	VAO.bind();

	VAO["posVBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["posVBO"].generate();
	VAO["posVBO"].bind();
	VAO["posVBO"].setData<glm::vec3>(positions.size(), positions.data(), GL_STATIC_DRAW);
	VAO["posVBO"].setAttPointer<glm::vec3>(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	VAO["normalVBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["normalVBO"].generate();
	VAO["normalVBO"].bind();
	VAO["normalVBO"].setData<glm::vec3>(normals.size(), normals.data(), GL_STATIC_DRAW);
	VAO["normalVBO"].setAttPointer<glm::vec3>(1, 3, GL_FLOAT, GL_TRUE, 0, 0);

	VAO["texVBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["texVBO"].generate();
	VAO["texVBO"].bind();
	VAO["texVBO"].setData<glm::vec2>(textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW);
	VAO["texVBO"].setAttPointer<glm::vec2>(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	VAO["EBO"].generate();
	VAO["EBO"].bind();
	VAO["EBO"].setData<GLuint>(indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	bv = new BoundingVolume(center, radius);
	bv->transform(this);
}

void Esfera::update(float dt) {
	float g = 9.81f;
	if (fixed) return;

	glm::vec3 ogPosition = position;
	position.x = ogPosition.x + velocity.x * cos(glm::radians(angle)) * dt;
	position.y = ogPosition.y + velocity.y * sin(glm::radians(angle)) * dt - 0.5 * g * dt * dt;
	direction = position - ogPosition;
	bv->transform(this);
}


void Esfera::moverse(glm::vec3 dir) {
	position += dir;
	bv->transform(this);
}


Caja::Caja(glm::vec3 position, glm::vec3 size) : Objeto::Objeto(position, size) {
	indices_size = 36;
}

Caja::Caja(Caja* caja, glm::vec3 position, glm::vec3 size) : Objeto::Objeto(position, size) {
	VAO = caja->VAO;
	indices_size = caja->indices_size;
	bv = new BoundingVolume(posmin, posmax);
	bv->transform(this);
}

void Caja::init() {
	positions = {
		glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f, -1.0f,  1.0f),

		glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(-1.0f,  1.0f,  1.0f),

		glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(1.0f,  1.0f,  1.0f),

		glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(-1.0f, -1.0f, -1.0f),

		glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f,  1.0f, -1.0f),
	};

	normals = {
		// normal
		glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f),

		glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f),
		glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f),

		glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(-1.0f,  0.0f,  0.0f),

		glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f),

		glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f),

		glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f),
	};

	textureCoords = {
		//texcoord
		  glm::vec2(0.0f, 0.0f),glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
		  glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),

		  glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
		  glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),

		  glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
		  glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),

		  glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
		  glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),

		  glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
		  glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f),

		  glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
		  glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)
	};

	for (int i = 0; i < indices_size; i++) {
		indices.push_back(i);
	}

	indices_size = indices.size();
}

void Caja::setup() {
	VAO = ArrayObject();

	VAO.generate();
	VAO.bind();

	VAO["posVBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["posVBO"].generate();
	VAO["posVBO"].bind();
	VAO["posVBO"].setData<glm::vec3>(positions.size(), positions.data(), GL_STATIC_DRAW);
	VAO["posVBO"].setAttPointer<glm::vec3>(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	VAO["normalVBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["normalVBO"].generate();
	VAO["normalVBO"].bind();
	VAO["normalVBO"].setData<glm::vec3>(normals.size(), normals.data(), GL_STATIC_DRAW);
	VAO["normalVBO"].setAttPointer<glm::vec3>(1, 3, GL_FLOAT, GL_TRUE, 0, 0);

	VAO["texVBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["texVBO"].generate();
	VAO["texVBO"].bind();
	VAO["texVBO"].setData<glm::vec2>(textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW);
	VAO["texVBO"].setAttPointer<glm::vec2>(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	VAO["EBO"].generate();
	VAO["EBO"].bind();
	VAO["EBO"].setData<GLuint>(indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	bv = new BoundingVolume(posmin, posmax);
	bv->transform(this);
}

void Caja::update(float dt) {
	float g = 9.81f;
	glm::vec3 ogPosition = position;
	position.x = ogPosition.x + velocity.x * cos(glm::radians(angle)) * dt;
	position.y = ogPosition.y + velocity.y * sin(glm::radians(angle)) * dt - 0.5 * g * dt * dt;
	direction = position - ogPosition;
	bv->transform(this);
}

void Caja::moverse(glm::vec3 dir) {
	position += dir;
	bv->transform(this);
}