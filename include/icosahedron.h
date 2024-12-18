#ifndef ICOSAHEDRON_H
#define ICOSAHEDRON_H

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

#include "shader.h"
#include "windowData.h"
#include "modelMatrix.h"

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

class Icosahedron : public Renderable
{
public:
	Icosahedron();

	void updateGeometry();

	std::vector<glm::vec3> m_data{};
	std::vector<glm::vec4> m_color_data{};

	void init(std::shared_ptr<WindowData> w) override;
	void renderSpecifics() override;
	void setUniforms() override;
	void populateVAO() override;
	void initGeometry() override;
private:
	float m_radius{1.0f};
	uint8_t m_SubdivisionLevel{3};
};

#endif