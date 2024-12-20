#include "../include/roomba.h"

Roomba::Roomba()
{
}

void Roomba::init(std::shared_ptr<WindowData> w)
{
	m_windowData = w;
	setupBuffers(2);
	initGeometry();
	populateVAO();
}

void Roomba::updateGeometry()
{
	// std::cout << m_currentEdge->point.x << ' ' << m_currentEdge->point.y << ' ' << m_currentEdge->point.z << '\n';
	// m_modelMatrix.m_position = m_currentEdge->point;
	// m_modelMatrix.updateAll();
}

void Roomba::nextEdge()
{
	// if (m_currentEdge == nullptr)
	// {
	// 	m_currentEdge = &(*m_adjacency)[0];
	// 	std::cout << m_currentEdge->neighbors.first->point.x << ' ' << m_currentEdge->neighbors.second->point.x << '\n';
	// 	if (m_currentEdge->wall)
	// 	{
	// 		m_currentEdge = m_currentEdge->neighbors.first->wall ? m_currentEdge->neighbors.second : m_currentEdge->neighbors.first;
	// 	}
	// 	updateGeometry();
	// 	return;
	// }

	// std::queue<Edge*> edgeQueue{};
	// std::unordered_set<Edge*> visited{};
	// edgeQueue.push(m_currentEdge);
	// visited.insert(m_currentEdge);
	// while (!edgeQueue.empty())
	// {
	// 	Edge* edge = edgeQueue.front();
	// 	edgeQueue.pop();

	// 	if (!edge->wall)
	// 	{
	// 		m_currentEdge = edge;
	// 		updateGeometry();
	// 		return;
	// 	}

	// 	if (edge->neighbors.first && visited.find(edge->neighbors.first) == visited.end())
	// 	{
	// 		edgeQueue.push(edge->neighbors.first);
	// 		visited.insert(edge->neighbors.first);
	// 	}
	// 	if (edge->neighbors.second && visited.find(edge->neighbors.second) == visited.end())
	// 	{
	// 		edgeQueue.push(edge->neighbors.second);
	// 		visited.insert(edge->neighbors.second);
	// 	}
	// }
}

void Roomba::initGeometry()
{
	m_data.push_back(glm::vec3{0.5f, -0.5f, 0.0f});
	m_data.push_back(glm::vec3{-0.5f, -0.5f, 0.0f});
	m_data.push_back(glm::vec3{0.0f, 0.5f, 0.0f});
	m_color_data.push_back(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
	m_color_data.push_back(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
	m_color_data.push_back(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
}

void Roomba::populateVAO()
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

void Roomba::renderSpecifics()
{
	glDrawArrays(GL_TRIANGLES, 0, m_data.size());
}

void Roomba::setUniforms()
{
	glm::mat4 mvpMatrix {m_windowData->m_perspective * m_windowData->m_view * m_modelMatrix.m_matrix};
	m_shader->setMat4("mvpMatrix", mvpMatrix);
}
