#ifndef WINDOWDATA_H
#define WINDOWDATA_H

#include <glm/glm.hpp>

struct WindowData
{
	glm::vec2 m_windowSize{1920, 1080};

	float m_viewRatio{m_windowSize.y/m_windowSize.x};

	glm::vec2 m_viewSize{m_windowSize.x*m_viewRatio, m_windowSize.y};
	glm::mat4 m_view{1.0f};
	glm::mat4 m_orthographic{1.0f};
	glm::mat4 m_perspective{1.0f};
	float m_aspectRatio{m_viewSize.x/m_viewSize.y};

	glm::vec2 m_center{m_viewSize.x/2, m_viewSize.y/2};

	glm::vec2 normalize(const glm::vec2 input)
	{
		return input / m_viewSize;
	}
};

#endif