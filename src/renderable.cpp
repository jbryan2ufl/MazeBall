#include "../include/renderable.h"

Renderable::Renderable()
{
}

void Renderable::setupBuffers(size_t count)
{
	m_VBOs.resize(count);

	glGenBuffers(count, m_VBOs.data());
	glGenVertexArrays(1, &m_VAO);
}

Renderable::~Renderable()
{
	glDeleteVertexArrays(1, &m_VAO);
	
	if (!m_VBOs.empty())
	{
		glDeleteBuffers(m_VBOs.size(), m_VBOs.data());
	}
}

void Renderable::setShader(std::shared_ptr<Shader> shader)
{
	m_shader = shader;
}

void Renderable::render()
{
	if (m_shader == nullptr)
	{
		return;
	}

	m_shader->use();
	setUniforms();
	glBindVertexArray(m_VAO);

	renderSpecifics();

	glBindVertexArray(0);
	glUseProgram(0);
}