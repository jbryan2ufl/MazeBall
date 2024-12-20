#ifndef ROOMBA_H
#define ROOMBA_H

#include <queue>
#include <unordered_set>

#include "mazeball.h"

class Roomba : public Renderable
{
public:

	Roomba();

	void setAdjacency(std::shared_ptr<AdjacencyMap> a) { m_adjacency = a; }

	void nextEdge();
	void updateGeometry();

	std::vector<glm::vec3> m_data{};
	std::vector<glm::vec4> m_color_data{};

	void init(std::shared_ptr<WindowData> w) override;
	void renderSpecifics() override;
	void setUniforms() override;
	void populateVAO() override;
	void initGeometry() override;

private:
	std::shared_ptr<AdjacencyMap> m_adjacency;
	Edge* m_currentEdge{};
};

#endif