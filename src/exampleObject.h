#ifndef EXAMPLEOBJECT_H
#define EXAMPLEOBJECT_H

#include "renderable.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <iostream>
#include <limits>
#include <memory>

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

class ExampleObject : public Renderable
{
public:
	ExampleObject();

	std::vector<glm::vec3> m_data{};
	std::vector<glm::vec4> m_color_data{};

	glm::vec3 m_base{0.0f, 0.0f, 0.0f};
	glm::vec3 m_head{0.0f, 0.0f, -1.0f};

	glm::vec4 m_color{1, 1, 1, 1};

	void updateGeometry();

	void init(std::shared_ptr<WindowData> w) override;
	void renderSpecifics() override;
	void setUniforms() override;
	void populateVAO() override;
	void initGeometry() override;
};

#endif