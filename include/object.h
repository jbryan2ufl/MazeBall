#ifndef OBJECT_H
#define OBJECT_H

#include <memory>

#include "modelMatrix.h"
#include "windowData.h"

class Object
{
public:
	Object()
		: m_modelMatrix{std::make_shared<ModelMatrix>()}
	{}

	virtual ~Object() = default;

protected:
	std::shared_ptr<ModelMatrix> m_modelMatrix{};
	std::shared_ptr<WindowData> m_windowData{nullptr};
};

#endif