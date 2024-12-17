#include "boundingBox.h"

BoundingBox::BoundingBox()
{
}

void BoundingBox::transform(const glm::mat4& modelMatrix)
{
	m_worldMin = glm::vec3(modelMatrix * glm::vec4(m_min, 1.0f));
	m_worldMax = glm::vec3(modelMatrix * glm::vec4(m_max, 1.0f));
}

bool BoundingBox::intersect(const Ray& ray) const
{
	float tmin = (m_worldMin.x - ray.origin.x) / ray.direction.x;
	float tmax = (m_worldMax.x - ray.origin.x) / ray.direction.x;

	if (tmin > tmax)
	{
		std::swap(tmin, tmax);
	}

	float tymin = (m_worldMin.y - ray.origin.y) / ray.direction.y;
	float tymax = (m_worldMax.y - ray.origin.y) / ray.direction.y;

	if (tymin > tymax)
	{
		std::swap(tymin, tymax);
	}

	if ((tmin > tymax) || (tymin > tmax))
	{
		return false;
	}

	if (tymin > tmin) 
	{
		tmin = tymin;
	}

	if (tymax < tmax)
	{
		tmax = tymax;
	}

	float tzmin = (m_worldMin.z - ray.origin.z) / ray.direction.z;
	float tzmax = (m_worldMax.z - ray.origin.z) / ray.direction.z;

	if (tzmin > tzmax)
	{
		std::swap(tzmin, tzmax);
	}

	if ((tmin > tzmax) || (tzmin > tmax))
	{
		return false;
	}

	return true;
}