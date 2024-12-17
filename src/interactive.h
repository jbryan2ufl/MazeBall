#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <glm/glm.hpp>

#include "boundingBox.h"
#include "object.h"
#include "ray.h"

class Interactive : public Object
{
public:
	Interactive();
	virtual ~Interactive() = default;

	void setBoundingBox(const glm::vec3 min, const glm::vec3 max);

	bool checkIntersection(const Ray& ray);

	BoundingBox m_boundingBox{};
protected:
};

#endif