#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm/glm.hpp>

#include "ray.h"

class BoundingBox
{
public:
	BoundingBox();

	void transform(const glm::mat4& modelMatrix);

	bool intersect(const Ray& ray) const;

	glm::vec3 m_min{};
	glm::vec3 m_max{};

private:
	glm::vec3 m_worldMin{};
	glm::vec3 m_worldMax{};
};

#endif