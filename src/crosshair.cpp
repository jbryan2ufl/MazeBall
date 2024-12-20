#include "../include/crosshair.h"

Crosshair::Crosshair()
{
}

void Crosshair::init(std::shared_ptr<WindowData> w)
{
	m_windowData = w;
	setupBuffers(2);
	initGeometry();
	populateVAO();
}

void Crosshair::renderSpecifics()
{
	glDrawArrays(GL_LINES, 0, m_data.size());
}

void Crosshair::setUniforms()
{
	glm::mat4 mvpMatrix {m_windowData->m_orthographic * glm::mat4{1.0f} * m_modelMatrix->m_matrix};
	m_shader->setMat4("mvpMatrix", mvpMatrix);
}

void Crosshair::populateVAO()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(glm::vec2), m_data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, m_color_data.size()*sizeof(glm::vec4), m_color_data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);
}

void Crosshair::initGeometry()
{
	m_modelMatrix->m_position = glm::vec3{m_windowData->m_center, 0.0f};
	m_modelMatrix->updateAll();

	m_data.push_back(glm::vec3{-m_length, 0.0f, 0.0f});
	m_data.push_back(glm::vec3{m_length, 0.0f, 0.0f});
	m_data.push_back(glm::vec3{0.0f, -m_length, 0.0f});
	m_data.push_back(glm::vec3{0.0f, m_length, 0.0f});
	m_color_data.push_back(m_color);
	m_color_data.push_back(m_color);
	m_color_data.push_back(m_color);
	m_color_data.push_back(m_color);
}
