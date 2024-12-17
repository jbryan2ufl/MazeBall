#ifndef TEXT_H
#define TEXT_H

#include <glm/glm.hpp>
#include <string>

#include "modelMatrix.h"

enum class TextAnchor
{
	TopLeft,
	TopCenter,
	TopRight,
	CenterLeft,
	Center,
	CenterRight,
	BottomLeft,
	BottomCenter,
	BottomRight
};

enum class TextJustification
{
	Left,
	Center,
	Right
};

enum class TextViewingMode
{
	Orthographic,
	Perspective
};

struct TextMetrics
{
	float maxCharHeight{};
	float fullTextHeight{};
	float fullTextWidth{};
};

struct Text
{
	std::string m_text{};
	ModelMatrix m_modelMatrix{};
	TextAnchor m_anchor{};
	TextJustification m_justification{};
	TextViewingMode m_viewingMode{};
	TextMetrics m_textMetrics{};
};

#endif