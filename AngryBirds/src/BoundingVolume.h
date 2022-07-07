#ifndef BOUNDINGVOLUME_H
#define BOUNDINGVOLUME_H

#include <glm/glm.hpp>

#include <algorithm>

#include "Objeto.h"

// se define un enum para separar los tipos de BoundingVolume
enum class BoundType : int {
	AABB = 1,
	SPHERE = 2
};

class BoundingVolume {
public:
	BoundType type;

	// si es SPHERE
	glm::vec3 center, ogCenter;
	float radius, ogRadius;

	// si es AABB
	glm::vec3 min, max;
	glm::vec3 ogMin, ogMax;

	// constructores
	BoundingVolume(BoundType type);
	BoundingVolume(glm::vec3 center, float radius);
	BoundingVolume(glm::vec3 min, glm::vec3 max);

	// acciones
	bool checkCollisions(BoundingVolume* bv);
	void transform(Esfera* sphere);
	void transform(Caja* caja);
	void transform(Medical* medical);
};

#endif