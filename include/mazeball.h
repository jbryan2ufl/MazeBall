#ifndef MAZEBALL_H
#define MAZEBALL_H

#include "renderable.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <iostream>
#include <limits>
#include <memory>
#include <math.h>
#include <random>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <array>

#include "shader.h"
#include "windowData.h"
#include "modelMatrix.h"
#include "unionFind.h"
// #include "mazeBallWalls.h"

class MazeBallWalls;

/*
for setup, call:
	- constructor(WindowData)
	- initShader(vs, fs) REPLACE WITH SHADER MANAGER
	- init() -> setupBuffers, initGeometry, populateVAO
	- if static, setUniforms()

to draw, call:
	- if dynamic, setUniforms()
	- render()
*/

struct uint16_PairHash {
    size_t operator()(const std::pair<uint16_t, uint16_t>& edge) const {
        // Hash the first element (int)
        size_t h1 = std::hash<uint16_t>()(edge.first);
        // Hash the second element (int)
        size_t h2 = std::hash<uint16_t>()(edge.second);
        
        // Combine the hashes using XOR and a shift to mix them
        return h1 ^ (h2 << 1);
    }
};

struct uint16_ArrayHash
{
    template <std::size_t N>
    std::size_t operator()(const std::array<uint16_t, N>& arr) const
	{
        std::size_t hash = 0;
        for (std::size_t i = 0; i < N; ++i)
		{
            hash ^= static_cast<std::size_t>(arr[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

struct uint16_Array2Hash {
    size_t operator()(const std::array<uint16_t, 2>& edge) const {
        return (static_cast<std::size_t>(edge[0]) << 16) | edge[1];
    }
};


struct Edge;
struct Triangle;

using TriangleIndex = uint16_t;
using EdgeIndex = uint16_t;
using VertexIndex = uint16_t;
using Vertex = glm::vec3;

#define INDEX_MAX UINT16_MAX

using AdjacencyMap = std::unordered_map<std::pair<EdgeIndex, TriangleIndex>, EdgeIndex, uint16_PairHash>;

struct Edge
{
	EdgeIndex index = INDEX_MAX;
	bool wall = true;
	glm::vec3 midpoint;
	std::array<VertexIndex, 2> vertices = {INDEX_MAX, INDEX_MAX};
	std::array<TriangleIndex, 2> triangles = {INDEX_MAX, INDEX_MAX};
};
struct Triangle
{
	TriangleIndex index = INDEX_MAX;
	std::array<VertexIndex, 3> vertices = {INDEX_MAX, INDEX_MAX, INDEX_MAX};
	std::array<EdgeIndex, 3> edges = {INDEX_MAX, INDEX_MAX, INDEX_MAX};
};

class MazeBall : public Renderable
{
public:

	MazeBall();

	void updateGeometry();

	std::shared_ptr<AdjacencyMap> getAdjacency() const { return m_adjacency; }

	std::vector<glm::vec3> m_data{};
	std::vector<glm::vec4> m_color_data{};

	void init(std::shared_ptr<WindowData> w) override;
	void renderSpecifics() override;
	void setUniforms() override;
	void populateVAO() override;
	void initGeometry() override;

	MazeBallWalls m_walls;

	friend class MazeBallWalls;

private:
	float m_radius{4.0f};
	uint8_t m_SubdivisionLevel{0};
	
	std::shared_ptr<AdjacencyMap> m_adjacency{};
	std::shared_ptr<std::vector<Triangle>> m_triangles{};
	std::shared_ptr<std::vector<Edge>> m_edges{};
	std::shared_ptr<std::vector<Vertex>> m_vertices{};

	size_t m_edgeCount{};
	size_t m_triangleCount{};

};

#endif
