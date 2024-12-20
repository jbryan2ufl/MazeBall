#include "../include/exampleObject.h"

ExampleObject::ExampleObject()
{
}

void ExampleObject::init(std::shared_ptr<WindowData> w)
{
	m_windowData = w;
	setupBuffers(2);
	initGeometry();
	populateVAO();
}

void ExampleObject::initGeometry()
{
	m_data.push_back(m_base);
	m_data.push_back(m_head);
	m_color_data.push_back(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
	m_color_data.push_back(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

void ExampleObject::updateGeometry()
{
	m_data.push_back(m_base);
	m_data.push_back(m_head);
	m_color_data.push_back(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
	m_color_data.push_back(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
	// m_data[0] = m_base;
	// m_data[1] = m_head;

	populateVAO();
}

void ExampleObject::populateVAO()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(glm::vec3), m_data.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, m_color_data.size()*sizeof(glm::vec4), m_color_data.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);
}

void ExampleObject::renderSpecifics()
{
	glDrawArrays(GL_LINES, 0, m_data.size());
}

void ExampleObject::setUniforms()
{
	glm::mat4 mvpMatrix {m_windowData->m_perspective * m_windowData->m_view * m_modelMatrix->m_matrix};
	m_shader->setMat4("mvpMatrix", mvpMatrix);
}