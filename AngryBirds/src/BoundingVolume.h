//
// Created by hgallegos on 23/05/2022.
//

#ifndef PROYECTO2_BOUNDINGVOLUME_H
#define PROYECTO2_BOUNDINGVOLUME_H

#include <glm/glm.hpp>

#include <algorithm>

#include "Objeto.h"


class BoundingBox;

class BoundingVolume {
public:
	virtual bool Colision(BoundingVolume& bv) = 0;
	virtual bool ColisionBox(BoundingBox* bv) = 0;
	virtual void calcular(Esfera& esf) = 0;
	virtual void calcular(Caja& caja) = 0;
};

class BoundingBox : public BoundingVolume {
public:
	glm::vec3 min, max;
	bool Colision(BoundingVolume& bv) { return false; }
	bool ColisionBox(BoundingBox* bv);
	void calcular(Esfera& esf);
	void calcular(Caja& caja);
};


class BoundingRegion {
public:
	int type; // 1 AABB, 2 SPHERE

	glm::vec3 min, max;
	glm::vec3 ogMin, ogMax;

	glm::vec3 center, ogCenter;
	float radius, ogRadius;

	RigidBody* instance;

	BoundingRegion(int type = 1);
	BoundingRegion(glm::vec3 center, float radius);
	BoundingRegion(glm::vec3 min, glm::vec3 max);

	void transform();
	glm::vec3 calculateCenter();
	glm::vec3 calculateDimensions();

	bool containsPoint(glm::vec3 pt);
	bool containsRegion(BoundingRegion br);
	bool intersectsWith(BoundingRegion br);
	bool operator==(BoundingRegion br);
};

#endif //PROYECTO2_BOUNDINGVOLUME_H