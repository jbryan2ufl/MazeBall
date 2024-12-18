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

struct pair_hash {
    size_t operator()(const std::pair<int, int>& edge) const {
        // Hash the first element (int)
        size_t h1 = std::hash<int>()(edge.first);
        // Hash the second element (int)
        size_t h2 = std::hash<int>()(edge.second);
        
        // Combine the hashes using XOR and a shift to mix them
        return h1 ^ (h2 << 1);
    }
};

struct Edge;
using Adjacency = std::vector<Edge>;
using AdjacencyMap = std::unordered_map<std::pair<int, int>, Edge*, pair_hash>;
struct Edge
{
	bool wall;
	glm::vec3 point;
	std::pair<Edge*, Edge*> neighbors;
};

class MazeBall : public Renderable
{
public:

	MazeBall();

	void updateGeometry();

	std::shared_ptr<Adjacency> getAdjacency() const { return m_adjacency; }

	std::vector<glm::vec3> m_data{};
	std::vector<glm::vec4> m_color_data{};

	void init(std::shared_ptr<WindowData> w) override;
	void renderSpecifics() override;
	void setUniforms() override;
	void populateVAO() override;
	void initGeometry() override;
private:
	float m_radius{4.0f};
	uint8_t m_SubdivisionLevel{0};
	std::shared_ptr<Adjacency> m_adjacency{};
	size_t m_edgeCount{};
};

#endif