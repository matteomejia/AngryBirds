#include "BoundingVolume.h"

// constructores
BoundingVolume::BoundingVolume(BoundType type)
	: type(type) {}
BoundingVolume::BoundingVolume(glm::vec3 min, glm::vec3 max)
	: type(BoundType::AABB), min(min), max(max), ogMin(min), ogMax(max) {}
BoundingVolume::BoundingVolume(glm::vec3 center, float radius)
	: type(BoundType::SPHERE), center(center), radius(radius), ogCenter(center), ogRadius(radius) {}

// actualizar valores con esfera
void BoundingVolume::transform(Esfera* sphere) {
	if (type == BoundType::AABB) {
		min = ogMin * sphere->size + sphere->position;
		max = ogMax * sphere->size + sphere->position;
	}
	else {
		center = ogCenter * sphere->size + sphere->position;
		radius = ogRadius * sphere->size.x;
	}
}

// actualizar valores con caja
void BoundingVolume::transform(Caja* caja) {
	if (type == BoundType::AABB) {
		min = ogMin * caja->size + caja->position;
		max = ogMax * caja->size + caja->position;
	}
	else {
		center = ogCenter * caja->size + caja->position;
		radius = ogRadius * caja->size.x;
	}
}

// probar colisiones, se tiene que probar segun las combinaciones de tipos
bool BoundingVolume::checkCollisions(BoundingVolume* bv) {
	if (type == BoundType::AABB && bv->type == BoundType::AABB) {
		return (min.x <= bv->max.x && max.x >= bv->min.x) &&
			(min.y <= bv->max.y && max.y >= bv->min.y) &&
			(min.z <= bv->max.z && max.z >= bv->min.z);
	}
	else if (type == BoundType::SPHERE && bv->type == BoundType::SPHERE) {
		return glm::length(center - bv->center) < (radius + bv->radius);
	}
	else if (type == BoundType::SPHERE) {
		float dist = 0.0f;
		for (int i = 0; i < 3; i++) {
			float closestPt = std::max(bv->min[i], std::min(center[i], bv->max[i]));
			dist += (closestPt - center[i]) * (closestPt - center[i]);
		}

		return dist < (radius* radius);
	}
	else {
		return bv->checkCollisions(this);
	}


}