#ifndef MAZEBALLWALLS_H
#define MAZEBALLWALLS_H

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
#include "mazeball.h"

class MazeBall;


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

class MazeBallWalls : public Renderable
{
public:
	MazeBallWalls(const MazeBall& mazeball);

	std::vector<glm::vec3> m_data{};

	void init(std::shared_ptr<WindowData> w) override;
	void renderSpecifics() override;
	void setUniforms() override;
	void populateVAO() override;
	void initGeometry() override;

private:
	float m_wallHeight{};
	float m_wallDepth{};
	float m_wallThickness{};
	const MazeBall& m_mb;
};

#endif
