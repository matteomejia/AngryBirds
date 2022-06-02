#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

// Estas clases abstraen las llamadas de vertex arrays y buffers
// La ventaja es que permite retirar las llamadas a openGL del main

// clase de buffer object
class BufferObject {
public:
	GLuint val;
	GLenum type;

	// constructores
	BufferObject() {}
	BufferObject(GLenum type)
		: type(type) {}

	// abstracciones
	void generate() {
		glGenBuffers(1, &val);
	}

	void bind() {
		glBindBuffer(type, val);
	}

	template<typename T>
	void setData(GLuint noElements, T* data, GLenum usage) {
		glBufferData(type, noElements * sizeof(T), data, usage);
	}

	template<typename T>
	void setAttPointer(GLuint idx, GLint size, GLenum type, GLboolean flip, GLuint stride, GLuint offset) {
		glVertexAttribPointer(idx, size, type, flip, stride * sizeof(T), (void*)(offset * sizeof(T)));
		glEnableVertexAttribArray(idx);
	}

	void clear() {
		glBindBuffer(type, 0);
	}

	void cleanup() {
		glDeleteBuffers(1, &val);
	}
};

// clase de vertex array object
class ArrayObject {
public:
	GLuint val;

	// abstracciones
	void generate() {
		glGenVertexArrays(1, &val);
	}

	void bind() {
		glBindVertexArray(val);
	}

	void draw(GLenum mode, GLuint count, GLenum type, GLuint* indices) {
		glDrawElements(mode, count, type, (void*)indices);
	}

	void cleanup() {
		glDeleteVertexArrays(1, &val);
	}

	static void clear() {
		glBindVertexArray(0);
	}
};

#endif