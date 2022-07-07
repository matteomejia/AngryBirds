#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include <sstream>
#include <fstream>

std::vector<glm::vec3> read_vertices_from_file(std::string filename) {
	std::vector<glm::vec3> points;

	std::ifstream file(filename);

	if (!file) {
		std::cerr << "Failed to load from " << filename << std::endl;
	}

	std::cout << "Reading " << filename << "..." << std::endl;

	float x, y, z;
	while (file >> x >> y >> z) {
		points.push_back(glm::vec3(x, y, z));
	}

	std::cout << "Finished reading " << filename << "..." << std::endl;

	return points;
}

std::vector<glm::vec3> read_normals_from_file(std::string filename) {
	std::vector<glm::vec3> points;

	std::ifstream file(filename);

	if (!file) {
		std::cerr << "Failed to load from " << filename << std::endl;
	}

	std::cout << "Reading " << filename << "..." << std::endl;

	float x, y, z;
	while (file >> x >> y >> z) {
		points.push_back(glm::vec3(x, y, z));
	}

	std::cout << "Finished reading " << filename << "..." << std::endl;
	
	return points;
}

std::vector<GLuint> read_uint_from_file(std::string filename) {
	std::vector<GLuint> numbers;

	std::ifstream file(filename);

	if (!file) {
		std::cerr << "Failed to load from " << filename << std::endl;
	}

	std::cout << "Reading " << filename << "..." << std::endl;

	GLuint x, y, z;
	while (file >> x >> y >> z) {
		numbers.push_back(x);
		numbers.push_back(y);
		numbers.push_back(z);
	}

	std::cout << "Finished reading " << filename << "..." << std::endl;

	return numbers;
}

#endif