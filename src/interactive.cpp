#include "interactive.h"

Interactive::Interactive()
{
}

void Interactive::setBoundingBox(const glm::vec3 min, const glm::vec3 max)
{
	m_boundingBox.m_min = min;
	m_boundingBox.m_max = max;
	m_boundingBox.transform(m_modelMatrix.m_matrix);
}

bool Interactive::checkIntersection(const Ray& ray)
{
	m_boundingBox.transform(m_modelMatrix.m_matrix);
	return m_boundingBox.intersect(ray);
}