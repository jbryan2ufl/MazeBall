#ifndef RENDERABLE_H
#define RENDERABLE_H

// generic opengl object
// has VAO/VBO, render function, init

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "shaderManager.h"
#include "windowData.h"
#include "modelMatrix.h"
#include "object.h"

class Renderable : public Object
{
public:
	Renderable();

	~Renderable();

	// init should call setupBuffers, initGeometry, populateVAO
	virtual void init(std::shared_ptr<WindowData> w) = 0;

	virtual void setUniforms() = 0;

	void render();

	void setShader(std::shared_ptr<Shader> shader);

protected:
	void setupBuffers(size_t count);

	virtual void initGeometry() = 0;

	virtual void populateVAO() = 0;

	virtual void renderSpecifics() = 0;

	unsigned int m_VAO{};
	std::vector<unsigned int> m_VBOs{};

	std::shared_ptr<Shader> m_shader;
};

#endif