#ifndef MODELMATRIX_H
#define MODELMATRIX_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

struct ModelMatrix
{
	void updateAll()
	{
		m_matrix = glm::mat4{1.0f};
		m_matrix = glm::translate(m_matrix, m_position);
		m_matrix = glm::scale(m_matrix, glm::vec3{m_scaleFactor});
		m_matrix = glm::rotate(m_matrix, m_angle, m_axis);
	}

	glm::mat4 m_matrix{1.0f};
	glm::mat4 m_translate{1.0f};
	glm::mat4 m_scale{1.0f};
	glm::mat4 m_rotate{1.0f};
	float m_angle{};
	glm::vec3 m_axis{0.0f, 0.0f, 1.0f};
	float m_scaleFactor{1.0f};
	glm::vec3 m_position{};
};

#endif