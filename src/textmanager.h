#pragma once

#include "imstb_truetype.h"
#include <unordered_map>
#include <memory>
#include "shader.h"
#include "modelMatrix.h"
#include "windowData.h"
#include "renderable.h"
#include "interactive.h"
#include "text.h"

/*
to setup, call:
	- constructor(WindowData)
	- initShader(vs, fs)
	- initFont(fontPath, fontSize)
	- initTextProperties(position, viewMode, anchor, justification)
	- generateText(text)
	- init()

in the future, TextManager will spawn a Text object which will be rendered
fonts will be loaded on init from fonts/ directory

should be
	- constructor(WindowData)
	- init() -> initFonts()
	- Text generateText(text)
*/

class TextManager : public Renderable, public Interactive
{
public:
	TextManager();
	~TextManager();

	void init(std::shared_ptr<WindowData> w) override;
	void renderSpecifics() override;
	void setUniforms() override;
	void populateVAO() override;
	void initGeometry() override;

	void initFont(const char* fontPath, float fontSize);
	void initTextProperties(glm::vec3 position, TextViewingMode v, TextAnchor a, TextJustification j);

	void generateText(std::string text);

	ModelMatrix m_modelMatrix{};

	void setJustification(TextJustification j);
	void setAnchor(TextAnchor a);

private:

	std::vector<Text> m_textObjects{};

	void loadFont(const char* fontPath);
	std::shared_ptr<WindowData> m_windowData{};

	std::string m_text{};

	TextViewingMode m_viewMode{TextViewingMode::Orthographic};

	void createTextureAtlas();

	GLuint textureAtlas;
	int atlasWidth = 512, atlasHeight = 512;
	std::vector<stbtt_bakedchar> charData{96}; // ASCII 32..126

	stbtt_fontinfo font;
	unsigned char* fontBuffer;

	unsigned int m_fontTexture{};

	TextMetrics m_textMetrics{};

	TextAnchor m_anchor{TextAnchor::Center};
	TextJustification m_justification{TextJustification::Center};

	void calculateFullTextSize(const std::vector<std::string> lines);
	float getLineWidth(const std::string line);

	float m_fontSize{};

	std::vector<float> m_vertices;

	std::vector<std::string> splitTextIntoLines(const std::string& text);
};
