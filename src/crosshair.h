#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "renderable.h"

class Crosshair : public Renderable
{
public:
	Crosshair();

	std::vector<glm::vec2> m_data{};
	std::vector<glm::vec4> m_color_data{};

	float m_length{5.0f};

	glm::vec4 m_color{1.0f, 1.0f, 0.0f, 0.5f};

	void init(std::shared_ptr<WindowData> w) override;
	void renderSpecifics() override;
	void setUniforms() override;
	void populateVAO() override;
	void initGeometry() override;
};

#endif