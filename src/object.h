#ifndef OBJECT_H
#define OBJECT_H

#include <memory>

#include "modelMatrix.h"
#include "windowData.h"

class Object
{
public:
	Object()
	{}

	virtual ~Object() = default;

protected:
	ModelMatrix m_modelMatrix{};
	std::shared_ptr<WindowData> m_windowData{nullptr};
};

#endif