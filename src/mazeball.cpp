#include "../include/mazeball.h"
#include "mazeBallWalls.h"

#define FRAND (float)rand()/RAND_MAX

MazeBall::MazeBall()
	: m_edgeCount{30*std::pow(4, m_SubdivisionLevel)}
	, m_triangleCount{20*std::pow(4, m_SubdivisionLevel)}
	, m_walls{*this}
{
	m_data.reserve(m_triangleCount*3);
	m_adjacency = std::make_shared<AdjacencyMap>();
	m_edges = std::make_shared<std::vector<Edge>>();
	m_triangles = std::make_shared<std::vector<Triangle>>();
	m_adjacency->reserve(m_edgeCount);
	m_edges->reserve(m_edgeCount);
	m_triangles->reserve(m_triangleCount);
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
	m_modelMatrix->m_angle += 0.005f/m_radius;
	m_modelMatrix->updateAll();
}

void MazeBall::initGeometry()
{

	auto& edges = *m_edges;
	auto& triangles = *m_triangles;

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

	/* http://patorjk.com/software/taag/#p=display&f=Sub-Zero&t=%0A
	 ______     __  __     ______     _____     __     __   __   __     _____     ______    
	/\  ___\   /\ \/\ \   /\  == \   /\  __-.  /\ \   /\ \ / /  /\ \   /\  __-.  /\  ___\   
	\ \___  \  \ \ \_\ \  \ \  __<   \ \ \/\ \ \ \ \  \ \ \'/   \ \ \  \ \ \/\ \ \ \  __\   
	 \/\_____\  \ \_____\  \ \_____\  \ \____-  \ \_\  \ \__|    \ \_\  \ \____-  \ \_____\ 
	  \/_____/   \/_____/   \/_____/   \/____/   \/_/   \/_/      \/_/   \/____/   \/_____/ 
	*/
	{
		std::unordered_map<std::pair<int, int>, int, uint16_PairHash> edgeToMidpointMap;

		auto getMidpoint = [&](int i1, int i2)
		{
			if (i1 > i2) std::swap(i1, i2);
			if (edgeToMidpointMap.find({i1, i2}) != edgeToMidpointMap.end())
			{
				return edgeToMidpointMap[{i1, i2}];
			}

			// calculate the midpoint between the two vertices
			glm::vec3 mid = glm::normalize((vertices[i1] + vertices[i2]) / 2.0f) * m_radius;
			vertices.push_back(mid);
			int newIndex = vertices.size() - 1;
			edgeToMidpointMap[{i1, i2}] = newIndex;
			return newIndex;
		};

		// fully subdivide sphere
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

	}

	/*
	 ______   ______     ______   __  __     __         ______     ______   ______    
	/\  == \ /\  __ \   /\  == \ /\ \/\ \   /\ \       /\  __ \   /\__  _\ /\  ___\   
	\ \  _-/ \ \ \/\ \  \ \  _-/ \ \ \_\ \  \ \ \____  \ \  __ \  \/_/\ \/ \ \  __\   
	 \ \_\    \ \_____\  \ \_\    \ \_____\  \ \_____\  \ \_\ \_\    \ \_\  \ \_____\ 
	  \/_/     \/_____/   \/_/     \/_____/   \/_____/   \/_/\/_/     \/_/   \/_____/ 
	*/
	{
		std::unordered_map<std::array<VertexIndex, 2>, EdgeIndex, uint16_Array2Hash> verticesToEdgeMap{};

		auto pushVertex = [&](VertexIndex v)
		{
			m_data.push_back(vertices[v]);
			m_color_data.push_back(glm::vec4(FRAND*0.2f+0.1f, 0.0f, FRAND*0.2f+0.1f, 1.0f));
		};

		auto setupEdge = [&](VertexIndex t0, VertexIndex t1, TriangleIndex tri) -> EdgeIndex
		{
			VertexIndex temp0{std::min(t0, t1)};
			VertexIndex temp1{std::max(t0, t1)};

			Edge* edgePtr;
			auto edgeIter{verticesToEdgeMap.find({temp0, temp1})};
			if (edgeIter != verticesToEdgeMap.end())
			{
				// if edge has already been generated, find it
				edgePtr = &edges[edgeIter->second];
			}
			else
			{
				// otherwise, create a new edge
				edges.push_back(Edge{});
				edgePtr = &edges.back();
				edgePtr->index = edges.size()-1;
				edgePtr->midpoint = 0.5f * (vertices[temp0] + vertices[temp1]);
				edgePtr->vertices = {temp0, temp1};

				verticesToEdgeMap[{temp0, temp1}] = edgePtr->index;
			}

			std::cout << "EDGE " << edgePtr->index << " NOW BEING ASSIGNED TO TRI " << tri << '\n';

			if (edgePtr->triangles[0] == -1)
			{
				edgePtr->triangles[0] = tri;
			}
			else
			{
				edgePtr->triangles[1] = tri;
			}

			return edgePtr->index;
		};

		auto setupTriangle = [&](VertexIndex t0, VertexIndex t1, VertexIndex t2)
		{
			triangles.push_back(Triangle{});

			Triangle& triRef{triangles[triangles.size()-1]};

			triRef.index = triangles.size();
			std::cout << "TRIANGLE " << triRef.index << " NOW BEING GENERATED\n";

			pushVertex(t0);
			pushVertex(t1);
			pushVertex(t2);

			triRef.vertices[0] = t0;
			triRef.vertices[1] = t1;
			triRef.vertices[2] = t2;

			triRef.edges[0] = setupEdge(t0, t1, triRef.index);
			triRef.edges[1] = setupEdge(t1, t2, triRef.index);
			triRef.edges[2] = setupEdge(t2, t0, triRef.index);

		};

		// populate triangle vertex data
		for (const auto& face : faces)
		{
			setupTriangle(face[0], face[1], face[2]);
		}
	}

	/*
	 __  __     ______     __  __     ______     __  __     ______     __        
	/\ \/ /    /\  == \   /\ \/\ \   /\  ___\   /\ \/ /    /\  __ \   /\ \       
	\ \  _"-.  \ \  __<   \ \ \_\ \  \ \___  \  \ \  _"-.  \ \  __ \  \ \ \____  
	 \ \_\ \_\  \ \_\ \_\  \ \_____\  \/\_____\  \ \_\ \_\  \ \_\ \_\  \ \_____\ 
	  \/_/\/_/   \/_/ /_/   \/_____/   \/_____/   \/_/\/_/   \/_/\/_/   \/_____/ 
	*/
	{

		std::random_device rd;
		std::mt19937 g{rd()};

		std::shuffle(edges.begin(), edges.end(), g);

		UnionFind uf(vertices.size());

		for (auto& edge : edges)
		{
			VertexIndex& v0{edge.vertices[0]};
			VertexIndex& v1{edge.vertices[1]};

			if (uf.find(v0) != uf.find(v1))
			{
				uf.unite(v0, v1);
				edge.wall = true;
			}
			else
			{
				edge.wall = false;
			}

			// add 
			if (edge.wall)
			{

			}
		}
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
	glm::mat4 mvpMatrix {m_windowData->m_perspective * m_windowData->m_view * m_modelMatrix->m_matrix};
	m_shader->setMat4("mvpMatrix", mvpMatrix);
}