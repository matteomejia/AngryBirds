#include "Objeto.h"
#include "BoundingVolume.h"

// constructores
Objeto::Objeto()
	: position(glm::vec3(0.0f)), size(glm::vec3(1.0f)), mass(1.0f), velocity(glm::vec3(0.0f)), direction(glm::vec3(0.0f)), acceleration(0.0f, -9.81f, 0.0f) {
	rotate = (0.0f) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (359.0f - (0.0f))));
}

Objeto::Objeto(glm::vec3 position, glm::vec3 size)
	: position(position), size(size), mass(1.0f), velocity(glm::vec3(0.0f)), direction(glm::vec3(0.0f)), acceleration(0.0f, -9.81f, 0.0f) {
	rotate = (0.0f) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (359.0f - (0.0f))));
}

// renderizar
void Objeto::display(Shader& sh) {
	model = glm::mat4(1.0f);
	model = glm::scale(model, size);
	model = glm::translate(model, position);
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 1.0, 0.0f));
	sh.setMat4("model", model);
	sh.setVec3("objectColor", color);

	VAO.bind();
	VAO.draw(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, indices.data());
	ArrayObject::clear();

}

// actualizar aceleracion
void Objeto::accelerate(glm::vec3 a) {
	acceleration += a;
}

// simular un impulso
void Objeto::push(float magnitude, glm::vec3 direction) {
	if (magnitude == 0) {
		return;
	}

	// KE = 1/2 * m * v^2
	glm::vec3 deltaV = sqrt(2 * abs(magnitude) / mass) * direction;
	velocity += magnitude > 0 ? deltaV : -deltaV;
}

// detectar colisiones
void Objeto::checkCollisions(std::vector<Objeto*> pObjetos) {
	for (Objeto* obj : pObjetos) {
		if (obj != this and bv->checkCollisions(obj->bv)) {
			obj->push(10.0f, velocity);
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

// aqui se estan usando las clases de Memory.h
void Esfera::setup() {
	VAO = ArrayObject();

	VAO.generate();
	VAO.bind();

	posVBO = BufferObject(GL_ARRAY_BUFFER);
	posVBO.generate();
	posVBO.bind();
	posVBO.setData<glm::vec3>(positions.size(), positions.data(), GL_STATIC_DRAW);
	posVBO.setAttPointer<glm::vec3>(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	normalVBO = BufferObject(GL_ARRAY_BUFFER);
	normalVBO.generate();
	normalVBO.bind();
	normalVBO.setData<glm::vec3>(normals.size(), normals.data(), GL_STATIC_DRAW);
	normalVBO.setAttPointer<glm::vec3>(1, 3, GL_FLOAT, GL_TRUE, 0, 0);

	texVBO = BufferObject(GL_ARRAY_BUFFER);
	texVBO.generate();
	texVBO.bind();
	texVBO.setData<glm::vec2>(textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW);
	texVBO.setAttPointer<glm::vec2>(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	EBO = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	EBO.generate();
	EBO.bind();
	EBO.setData<GLuint>(indices.size(), indices.data(), GL_STATIC_DRAW);

	// unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// crear y calcular bounding volume
	bv = new BoundingVolume(center, radius);
	bv->transform(this);
}

// actualizar posicion, velocidad y bounding volume
void Esfera::update(float dt) {
	position += velocity * dt + 0.5f * acceleration * (dt * dt);
	velocity += acceleration * dt;
	// limite ficticio de caida
	if (position.y < y_limit) {
		position.y = y_limit;
	}
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
		// vertices
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
		// normales
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
		// texturas
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

// usando clases de Memory.h
void Caja::setup() {
	VAO = ArrayObject();

	VAO.generate();
	VAO.bind();

	posVBO = BufferObject(GL_ARRAY_BUFFER);
	posVBO.generate();
	posVBO.bind();
	posVBO.setData<glm::vec3>(positions.size(), positions.data(), GL_STATIC_DRAW);
	posVBO.setAttPointer<glm::vec3>(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	normalVBO = BufferObject(GL_ARRAY_BUFFER);
	normalVBO.generate();
	normalVBO.bind();
	normalVBO.setData<glm::vec3>(normals.size(), normals.data(), GL_STATIC_DRAW);
	normalVBO.setAttPointer<glm::vec3>(1, 3, GL_FLOAT, GL_TRUE, 0, 0);

	texVBO = BufferObject(GL_ARRAY_BUFFER);
	texVBO.generate();
	texVBO.bind();
	texVBO.setData<glm::vec2>(textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW);
	texVBO.setAttPointer<glm::vec2>(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	EBO = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	EBO.generate();
	EBO.bind();
	EBO.setData<GLuint>(indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	bv = new BoundingVolume(posmin, posmax);
	bv->transform(this);
}

// actualizar posicion, velocidad y bounding volume
void Caja::update(float dt) {
	position += velocity * dt + 0.5f * acceleration * (dt * dt);
	velocity += acceleration * dt;
	if (position.y < y_limit) {
		position.y = y_limit;
	}
	bv->transform(this);
}