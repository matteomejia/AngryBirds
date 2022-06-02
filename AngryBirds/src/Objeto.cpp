//
// Created by hgallegos on 20/05/2022.
//

#include "Objeto.h"
#include "BoundingVolume.h"

void Objeto::addInstance(glm::vec3 size, float mass, glm::vec3 pos) {
	RigidBody* rb = new RigidBody(size, mass, pos);
	instances.push_back(rb);
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
}

GLuint Esfera::setup() {
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

	indices_size = indices.size();

	addInstance(glm::vec3(1.0f), 1.0f, glm::vec3(0.0f));

	return vao;
}

void Esfera::display(Shader& sh) {
	model = glm::mat4(1.0);
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::translate(model, centro);
	sh.setMat4("model", model);
	if (visible) {
		VAO.bind();
		VAO.draw(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, indices.data());
		ArrayObject::clear();
	}
}

void Esfera::actualizarDatos(float t) {
	float g = 9.81;
	glm::vec3 tmp = centro;
	centro.x = pos_ini.x + vel_ini.x * cos(glm::radians(ang_ini)) * t;
	centro.y = pos_ini.y + vel_ini.y * sin(glm::radians(ang_ini)) * t - 0.5 * g * t * t;
	dir = centro - tmp;
	bv->calcular(*this);
	std::cout << t << "\t" << to_string(pos_ini) << "\t" << to_string(centro) << std::endl;
}

void Esfera::calcularColision(std::vector<Objeto*> pObjetos) {
	for (auto& obj : pObjetos) {
		if (this != obj and bv->ColisionBox(static_cast<BoundingBox*>(obj->bv))) {
			// reacciónar a la colision
			std::cout << "Hay colision \n";
			obj->moverse(dir);
		}
	}
}
void Esfera::moverse(glm::vec3 dir) {
	centro += dir;
	bv->calcular(*this);
}

void Caja::init() {
	glm::vec3 esquina1 = glm::vec3(posmin.x, posmin.y, posmin.z); // - - -
	glm::vec3 esquina2 = glm::vec3(posmin.x, posmin.y, posmax.z); // - - +
	glm::vec3 esquina3 = glm::vec3(posmax.x, posmin.y, posmin.z); // + - -
	glm::vec3 esquina4 = glm::vec3(posmax.x, posmin.y, posmax.z); // + - +

	glm::vec3 esquina5 = glm::vec3(posmin.x, posmax.y, posmin.z); // - + -
	glm::vec3 esquina6 = glm::vec3(posmin.x, posmax.y, posmax.z); // - + +
	glm::vec3 esquina7 = glm::vec3(posmax.x, posmax.y, posmin.z); // + + -
	glm::vec3 esquina8 = glm::vec3(posmax.x, posmax.y, posmax.z); // + + +

	positions = {
		esquina1, esquina3, esquina7,
		esquina7, esquina5, esquina1,

		esquina2, esquina4, esquina8,
		esquina8, esquina6, esquina2,

		esquina6, esquina5, esquina1,
		esquina1, esquina2, esquina6,

		esquina8, esquina7, esquina3,
		esquina3, esquina4, esquina8,

		esquina1, esquina3, esquina4,
		esquina4, esquina2, esquina1,

		esquina5, esquina7, esquina8,
		esquina8, esquina6, esquina5
	};

	glm::vec3 normal1 = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 normal2 = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 normal3 = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 normal4 = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 normal5 = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 normal6 = glm::vec3(-1.0f, 0.0f, 0.0f);

	normals = {
		normal2, normal2, normal2, normal2, normal2, normal2,
		normal1, normal1, normal1, normal1, normal1, normal1,
		normal6, normal6, normal6, normal6, normal6, normal6,
		normal5, normal5, normal5, normal5, normal5, normal5,
		normal4, normal4, normal4, normal4, normal4, normal4,
		normal3, normal3, normal3, normal3, normal3, normal3
	};

	glm::vec2 tex1 = glm::vec2(0.0f, 0.0f);
	glm::vec2 tex2 = glm::vec2(0.0f, 1.0f);
	glm::vec2 tex3 = glm::vec2(1.0f, 0.0f);
	glm::vec2 tex4 = glm::vec2(1.0f, 1.0f);

	textureCoords = {
		tex1, tex3, tex4, tex4, tex2, tex1,
		tex1, tex3, tex4, tex4, tex2, tex1,
		tex3, tex4, tex2, tex2, tex1, tex3,
		tex3, tex4, tex2, tex2, tex1, tex3,
		tex2, tex4, tex3, tex3, tex1, tex2,
		tex2, tex4, tex3, tex3, tex1, tex2,
	};

	for (int i = 0; i < indices_size; i++) {
		indices.push_back(i);
	}
}

GLuint Caja::setup() {
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

	indices_size = indices.size();

	return vao;
}

void Caja::display(Shader& sh) {
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.0f));
	model = glm::translate(model, pos_ini);
	sh.setMat4("model", model);
	if (visible) {
		VAO.bind();
		VAO.draw(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, indices.data());
		ArrayObject::clear();
	}
}

void Caja::actualizarDatos(float t) {
	float g = 9.81f;

}

void Caja::calcularColision(std::vector<Objeto*> pObjetos) {}

void Caja::moverse(glm::vec3 dir) {}