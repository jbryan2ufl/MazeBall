#include "../include/mazeball.h"

#define FRAND (float)rand()/RAND_MAX

MazeBall::MazeBall()
	: m_edgeCount{30*std::pow(4, m_SubdivisionLevel)}
{
	m_adjacency = std::make_shared<Adjacency>();
	m_adjacency->reserve(m_edgeCount);
}

void MazeBall::init(std::shared_ptr<WindowData> w)
{
	m_windowData = w;
	setupBuffers(2);
	initGeometry();
	populateVAO();
}

void MazeBall::updateGeometry()
{
	m_modelMatrix.m_angle += 0.005f/m_radius;
	m_modelMatrix.updateAll();
}

void MazeBall::initGeometry()
{

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

	for (const auto& face : faces)
	{
		for (int index : face)
		{
			m_data.push_back(vertices[index]);
			m_color_data.push_back(glm::vec4(FRAND*0.2f+0.1f, 0.0f, FRAND*0.2f+0.1f, 1.0f));
		}
	}

	std::set<std::pair<int, int>> uniqueEdges;
	std::vector<std::pair<int, int>> mazeEdges;
	AdjacencyMap map{};

	// Collect edges from each face (triangle)
	for (const auto& face : faces) {
		int a = face[0], b = face[1], c = face[2];
		int x = 0, y = 0;


		x = std::min(a,b);
		y = std::max(a,b);
		glm::vec3 midpoint{0.5f * (vertices[x] + vertices[y])};

		Edge e1{true, midpoint, {nullptr, nullptr}};
		Edge e2{true, {}, {nullptr, nullptr}};
		Edge e3{true, {}, {nullptr, nullptr}};
		e1.neighbors = {&e2, &e3};
		e2.neighbors = {&e1, &e3};
		e3.neighbors = {&e1, &e2};

		auto result = uniqueEdges.insert({x, y});
		if (result.second)
		{
			m_adjacency->push_back(e1);
			map[{x, y}] = &m_adjacency->back();
		}

		x = std::min(b,c);
		y = std::max(b,c);
		midpoint = 0.5f * (vertices[x] + vertices[y]);
		
		if (result.second)
		{
			e2.point = midpoint;
			result = uniqueEdges.insert({x, y});
			m_adjacency->push_back(e2);
			map[{x, y}] = &m_adjacency->back();
		}


		x = std::min(c,a);
		y = std::max(c,a);
		result = uniqueEdges.insert({x, y});
		if (result.second)
		{
			midpoint = 0.5f * (vertices[x] + vertices[y]);
			e3.point = midpoint;
			m_adjacency->push_back(e3);
			map[{x, y}] = &m_adjacency->back();
		}
	}

	std::cout << "SIZE " << m_adjacency->size() << '\n';

	// for (const auto& edge : *m_adjacency)
	// {
	// 	std::cout << edge.point.x << ' ' << edge.point.y << ' ' << edge.point.z << '\n';
	// }

	std::vector<std::pair<int, int>> edges(uniqueEdges.begin(), uniqueEdges.end());

	std::random_device rd;
	std::mt19937 g{rd()};

	std::shuffle(edges.begin(), edges.end(), g);

	UnionFind uf(vertices.size());  // Create a union-find structure for all the vertices

	// Process each edge
	for (size_t i{}; i < edges.size(); i++)
	{
		int v1 = edges[i].first, v2 = edges[i].second;

		if (uf.find(v1) != uf.find(v2))
		{
			uf.unite(v1, v2);
			mazeEdges.push_back(edges[i]);
		}
		else
		{
			map[{v1, v2}]->wall = false;
		}
	}

	float wallHeight = 0.05f * m_radius;
	float wallDepth = -0.2f * m_radius;
	float wallThickness = 0.005f * m_radius;

	for (auto& index_pair : mazeEdges)
	{
		glm::vec3 alongWall{vertices[index_pair.first] - vertices[index_pair.second]};
		glm::vec3 out{glm::normalize(vertices[index_pair.first] - m_modelMatrix.m_position)};
		glm::vec3 cross{glm::normalize(glm::cross(alongWall, out))};
		glm::vec3 wallHeightOffset{out*wallHeight};
		glm::vec3 wallDepthOffset{out*wallDepth};

		// top tri 1
		m_data.push_back(vertices[index_pair.second]+wallHeightOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallHeightOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset+cross*wallThickness);

		// tri 1 side
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallHeightOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallDepthOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallDepthOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallDepthOffset+cross*wallThickness);

		// top tri 2
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallHeightOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset+cross*wallThickness);

		// tri 2 side
		m_data.push_back(vertices[index_pair.second]+wallHeightOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallDepthOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallDepthOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallDepthOffset-cross*wallThickness);

		// end cap 1
		m_data.push_back(vertices[index_pair.second]+wallHeightOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallHeightOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallDepthOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallHeightOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallDepthOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.second]+wallDepthOffset-cross*wallThickness);
		// end cap 2
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallDepthOffset-cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallHeightOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallDepthOffset+cross*wallThickness);
		m_data.push_back(vertices[index_pair.first]+wallDepthOffset-cross*wallThickness);
		for (int i{}; i < 30; i++)
		{
			m_color_data.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));
		}

		// m_data.push_back(vertices[index_pair.first]);
		// m_data.push_back(vertices[index_pair.second]);
		// m_data.push_back(vertices[index_pair.first]*(1+wallHeight));
		// m_data.push_back(vertices[index_pair.first]*(1+wallHeight));
		// m_data.push_back(vertices[index_pair.second]);
		// m_data.push_back(vertices[index_pair.second]*(1+wallHeight));
		// for (int i{}; i < 6; i++)
		// {
		// 	m_color_data.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 0.6f));
		// }

	}

}

void MazeBall::populateVAO()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(glm::vec3), m_data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, m_color_data.size()*sizeof(glm::vec4), m_color_data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);
}

void MazeBall::renderSpecifics()
{
	glDrawArrays(GL_TRIANGLES, 0, m_data.size());
}

void MazeBall::setUniforms()
{
	glm::mat4 mvpMatrix {m_windowData->m_perspective * m_windowData->m_view * m_modelMatrix.m_matrix};
	m_shader->setMat4("mvpMatrix", mvpMatrix);
}