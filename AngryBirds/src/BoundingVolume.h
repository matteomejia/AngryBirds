//
// Created by hgallegos on 23/05/2022.
//

#ifndef PROYECTO2_BOUNDINGVOLUME_H
#define PROYECTO2_BOUNDINGVOLUME_H

#include <glm/glm.hpp>

#include <algorithm>

#include "Objeto.h"

enum class BoundType : int {
	AABB = 1,
	SPHERE = 2
};

class BoundingVolume {
public:
	BoundType type;

	glm::vec3 center, ogCenter;
	float radius, ogRadius;

	glm::vec3 min, max;
	glm::vec3 ogMin, ogMax;

	BoundingVolume(BoundType type);
	BoundingVolume(glm::vec3 center, float radius);
	BoundingVolume(glm::vec3 min, glm::vec3 max);

	bool checkCollision(BoundingVolume* bv);
	void transform(Esfera* sphere);
	void transform(Caja* caja);
};

#endif //PROYECTO2_BOUNDINGVOLUME_H