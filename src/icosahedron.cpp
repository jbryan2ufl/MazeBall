#include "../include/icosahedron.h"

#define FRAND (float)rand()/RAND_MAX

struct pair_hash {
	template <typename T1, typename T2>
	std::size_t operator ()(const std::pair<T1, T2>& p) const
	{
		std::size_t h1 = std::hash<T1>{}(p.first);
		std::size_t h2 = std::hash<T2>{}(p.second);
		return h1 ^ (h2 << 1);
	}
};

Icosahedron::Icosahedron()
{
	
}

void Icosahedron::init(std::shared_ptr<WindowData> w)
{
	m_windowData = w;
	setupBuffers(2);
	initGeometry();
	populateVAO();
}

void Icosahedron::updateGeometry()
{
	m_modelMatrix.m_position = glm::vec3{0.0f, 1.5f, 0.0f};
	m_modelMatrix.m_angle += 0.01f;
	m_modelMatrix.updateAll();
}

void Icosahedron::initGeometry()
{
	// m_data push back vertex location
	// m_color_data push back color data

	float golden_ratio = glm::golden_ratio<float>();
	std::vector<glm::vec3> vertices
	{
		{-1.0f,			 golden_ratio,	 0.0f},
		{ 1.0f,			 golden_ratio,	 0.0f},
		{-1.0f,			-golden_ratio,	 0.0f},
		{ 1.0f,			-golden_ratio,	 0.0f},
		{ 0.0f,			-1.0f,			 golden_ratio},
		{ 0.0f,			 1.0f,			 golden_ratio},
		{ 0.0f,			-1.0f,			-golden_ratio},
		{ 0.0f,			 1.0f,			-golden_ratio},
		{ golden_ratio,	 0.0f,			-1.0f},
		{ golden_ratio,	 0.0f,			 1.0f},
		{-golden_ratio,	 0.0f,			-1.0f},
		{-golden_ratio,	 0.0f,			 1.0f}
	};

	for (auto& vertex : vertices)
	{
		vertex = normalize(vertex) * m_radius;
	}

	std::vector<std::vector<int>> faces =
	{
		{0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
		{1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
		{3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
		{4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
	};

	std::unordered_map<std::pair<int, int>, int, pair_hash> edge_map;

	auto getMidpoint = [&](int i1, int i2) {
        if (i1 > i2) std::swap(i1, i2);  // Make the order consistent
        if (edge_map.find({i1, i2}) != edge_map.end()) {
            return edge_map[{i1, i2}];
        }

        // Calculate the midpoint between the two vertices
        glm::vec3 mid = glm::normalize((vertices[i1] + vertices[i2]) / 2.0f) * m_radius;
        vertices.push_back(mid);
        int newIndex = vertices.size() - 1;
        edge_map[{i1, i2}] = newIndex;
        return newIndex;
    };

	for (int i = 0; i < m_SubdivisionLevel; ++i)
		{
			std::vector<std::vector<int>> newFaces;

			for (const auto& face : faces)
			{
				int a = face[0], b = face[1], c = face[2];

				int ab = getMidpoint(a, b);
				int bc = getMidpoint(b, c);
				int ca = getMidpoint(c, a);

				newFaces.push_back({a, ab, ca});
				newFaces.push_back({b, bc, ab});
				newFaces.push_back({c, ca, bc});
				newFaces.push_back({ab, bc, ca});
			}

			faces = std::move(newFaces);
	}

	size_t i{0};
	for (const auto& face : faces)
	{
		for (int index : face)
		{
			m_data.push_back(vertices[index]);
			// m_color_data.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 0.2f));
			m_color_data.push_back(glm::vec4(FRAND, FRAND, FRAND, 0.5f));
		}
			i++;
	}

	std::cout << i << '\n';
}

void Icosahedron::populateVAO()
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

void Icosahedron::renderSpecifics()
{
	glDrawArrays(GL_TRIANGLES, 0, m_data.size());
}

void Icosahedron::setUniforms()
{
	glm::mat4 mvpMatrix {m_windowData->m_perspective * m_windowData->m_view * m_modelMatrix.m_matrix};
	m_shader->setMat4("mvpMatrix", mvpMatrix);
}