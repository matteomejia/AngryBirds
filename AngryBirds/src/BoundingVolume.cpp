//
// Created by hgallegos on 23/05/2022.
//

#include "BoundingVolume.h"

bool BoundingBox::ColisionBox(BoundingBox* b) {

	return (min.x <= b->max.x && max.x >= b->min.x) &&
		(min.y <= b->max.y && max.y >= b->min.y) &&
		(min.z <= b->max.z && max.z >= b->min.z);
}

void BoundingBox::calcular(Esfera& esf) {
	min = esf.centro - glm::vec3(esf.radius);
	max = esf.centro + glm::vec3(esf.radius);
}

void BoundingBox::calcular(Caja& caja) {
	min = caja.posmin;
	max = caja.posmax;
}

// initialize with type
BoundingRegion::BoundingRegion(int type)
	: type(type) {}

// initialize as sphere
BoundingRegion::BoundingRegion(glm::vec3 center, float radius)
	: type(2), center(center), radius(radius), ogCenter(center), ogRadius(radius) {}

// initialize as AABB
BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max)
	: type(1), min(min), max(max), ogMin(min), ogMax(max) {}


void BoundingRegion::transform() {
	if (instance) {
		if (type == 1) {
			min = ogMin * instance->size + instance->pos;
			max = ogMax * instance->size + instance->pos;
		}
		else {
			center = ogCenter * instance->size + instance->pos;
			radius = ogRadius * instance->size.x;
		}
	}
}

glm::vec3 BoundingRegion::calculateCenter() {
	return (type == 1) ? (min + max) / 2.0f : center;
}

glm::vec3 BoundingRegion::calculateDimensions() {
	return (type == 1) ? (max - min) : glm::vec3(2.0f * radius);
}


bool BoundingRegion::containsPoint(glm::vec3 pt) {
	if (type == 1) {
		// box - point must be larger than man and smaller than max
		return (pt.x >= min.x) && (pt.x <= max.x) &&
			(pt.y >= min.y) && (pt.y <= max.y) &&
			(pt.z >= min.z) && (pt.z <= max.z);
	}
	else {
		// sphere - distance must be less than radius
		// x^2 + y^2 + z^2 <= r^2
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++) {
			distSquared += (center[i] - pt[i]) * (center[i] - pt[i]);
		}
		return distSquared <= (radius * radius);
	}
}

bool BoundingRegion::containsRegion(BoundingRegion br) {
	if (br.type == 1) {
		// if br is a box, just has to contain min and max
		return containsPoint(br.min) && containsPoint(br.max);
	}
	else if (type == 2 && br.type == 2) {
		// if both spheres, combination of distance from centers and br.radius is less than radius
		return glm::length(center - br.center) + br.radius < radius;
	}
	else {
		// if this is a box and br is a sphere

		if (!containsPoint(br.center)) {
			// center is outside of the box
			return false;
		}

		// center inside the box
		/*
			for each axis (x, y, z)
			- if distance to each side is smaller than the radius, return false
		*/
		for (int i = 0; i < 3; i++) {
			if (abs(max[i] - br.center[i]) < br.radius ||
				abs(br.center[i] - min[i]) < br.radius) {
				return false;
			}
		}

		return true;
	}
}

bool BoundingRegion::intersectsWith(BoundingRegion br) {

	if (type == 1 && br.type == 1) {

		glm::vec3 rad = calculateDimensions() / 2.0f;				// "radius" of this box
		glm::vec3 radBr = br.calculateDimensions() / 2.0f;			// "radius" of br

		glm::vec3 center = calculateCenter();						// center of this box
		glm::vec3 centerBr = br.calculateCenter();					// center of br

		glm::vec3 dist = abs(center - centerBr);

		for (int i = 0; i < 3; i++) {
			if (dist[i] > rad[i] + radBr[i]) {
				// no overlap on this axis
				return false;
			}
		}

		// failed to prove wrong on each axis
		return true;
	}
	else if (type == 2 && br.type == 2) {
		// both spheres - distance between centers must be less than combined radius

		return glm::length(center - br.center) < (radius + br.radius);
	}
	else if (type == 2) {
		// this is a sphere, br is a box

		// determine if sphere is above top, below bottom, etc
		// find distance (squared) to the closest plane
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++) {
			float closestPt = std::max(br.min[i], std::min(center[i], br.max[i]));
			distSquared += (closestPt - center[i]) * (closestPt - center[i]);
		}

		return distSquared < (radius* radius);
	}
	else {
		// this is a box, br is a sphere
		// call algorithm for br (defined in preceding else if block)
		return br.intersectsWith(*this);
	}
}

bool BoundingRegion::operator==(BoundingRegion br) {
	if (type != br.type) {
		return false;
	}

	if (type == 1) {
		return min == br.min && max == br.max;
	}
	else {
		return center == br.center && radius == br.radius;
	}
}