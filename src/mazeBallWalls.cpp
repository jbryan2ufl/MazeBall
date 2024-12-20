#include "../include/mazeBallWalls.h"

MazeBallWalls::MazeBallWalls(const MazeBall& mb)
	: m_mb{mb}
	, m_wallHeight{0.05f * mb.m_radius}
	, m_wallDepth{-0.2f * mb.m_radius}
	, m_wallThickness{0.005f * mb.m_radius}
{
	m_data.reserve(mb.m_edgeCount*30);
	m_modelMatrix = m_mb.m_modelMatrix;
}

void MazeBallWalls::init(std::shared_ptr<WindowData> w)
{
	m_windowData = w;
	setupBuffers(1);
	initGeometry();
	populateVAO();
}

void MazeBallWalls::initGeometry()
{
	const auto& vertices{*m_mb.m_vertices};
	const auto& edges{*m_mb.m_edges};

	for (auto& edge : edges)
	{
		if (!edge.wall)
		{
			continue;
		}

		const Vertex& v0{vertices[edge.vertices[0]]};
		const Vertex& v1{vertices[edge.vertices[1]]};

		glm::vec3 alongWall{v0 - v1};
		glm::vec3 out{glm::normalize(v0 - m_modelMatrix->m_position)};
		glm::vec3 cross{glm::normalize(glm::cross(alongWall, out))*m_wallThickness};
		glm::vec3 wallHeightOffset{out*m_wallHeight};
		glm::vec3 wallDepthOffset{out*m_wallDepth};

		// top of the wall
		// t1
		m_data.push_back(v1 + wallHeightOffset - cross);
		m_data.push_back(v1 + wallHeightOffset + cross);
		m_data.push_back(v0 + wallHeightOffset + cross);
		// t2
		m_data.push_back(v0 + wallHeightOffset - cross);
		m_data.push_back(v1 + wallHeightOffset - cross);
		m_data.push_back(v0 + wallHeightOffset + cross);

		// sides of the wall
		// s1 t1
		m_data.push_back(v0 + wallHeightOffset + cross);
		m_data.push_back(v1 + wallHeightOffset + cross);
		m_data.push_back(v1 + wallDepthOffset + cross);
		// s1 t2
		m_data.push_back(v0 + wallHeightOffset + cross);
		m_data.push_back(v1 + wallDepthOffset + cross);
		m_data.push_back(v0 + wallDepthOffset + cross);
		// s2 t1
		m_data.push_back(v1 + wallHeightOffset - cross);
		m_data.push_back(v0 + wallHeightOffset - cross);
		m_data.push_back(v1 + wallDepthOffset - cross);
		// s2 t2
		m_data.push_back(v1 + wallDepthOffset - cross);
		m_data.push_back(v0 + wallHeightOffset - cross);
		m_data.push_back(v0 + wallDepthOffset - cross);

		// end caps
		// e1 t1
		m_data.push_back(v1 + wallHeightOffset - cross);
		m_data.push_back(v1 + wallHeightOffset + cross);
		m_data.push_back(v1 + wallHeightOffset - cross);
		// e1 t2
		m_data.push_back(v1 + wallHeightOffset + cross);
		m_data.push_back(v1 + wallHeightOffset + cross);
		m_data.push_back(v1 + wallHeightOffset - cross);
		// e2 t1
		m_data.push_back(v0 + wallHeightOffset - cross);
		m_data.push_back(v0 + wallHeightOffset + cross);
		m_data.push_back(v0 + wallHeightOffset - cross);
		// e2 t2
		m_data.push_back(v0 + wallHeightOffset + cross);
		m_data.push_back(v0 + wallHeightOffset + cross);
		m_data.push_back(v0 + wallHeightOffset - cross);
	}
}

void MazeBallWalls::populateVAO()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(glm::vec3), m_data.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);
}

void MazeBallWalls::renderSpecifics()
{
	glDrawArrays(GL_TRIANGLES, 0, m_data.size());
}

void MazeBallWalls::setUniforms()
{
	glm::mat4 mvpMatrix {m_windowData->m_perspective * m_windowData->m_view * m_modelMatrix->m_matrix};
	m_shader->setMat4("mvpMatrix", mvpMatrix);
}