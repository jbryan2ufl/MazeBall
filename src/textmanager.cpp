#include "textmanager.h"

TextManager::TextManager()
	: Renderable{}
{
}

void TextManager::initFont(const char* fontPath, float fontSize)
{
	m_fontSize = fontSize;
	loadFont(fontPath);
	createTextureAtlas();
}

void TextManager::initTextProperties(glm::vec3 position, TextViewingMode v, TextAnchor a, TextJustification j)
{
	m_viewMode = v;
	m_anchor = a;
	m_justification = j;
	if (m_viewMode == TextViewingMode::Perspective)
	{
		m_modelMatrix.m_scaleFactor = 0.01f;
	}
	m_modelMatrix.m_position = position;
	m_modelMatrix.updateAll();
}

void TextManager::loadFont(const char* fontPath)
{
	FILE* fontFile = fopen(fontPath, "rb");
	if (!fontFile)
	{
		std::cerr << "Could not open font file: " << fontPath << std::endl;
		return;
	}

	fseek(fontFile, 0, SEEK_END);
	size_t fontFileSize = ftell(fontFile);
	fseek(fontFile, 0, SEEK_SET);

	fontBuffer = new unsigned char[fontFileSize];
	fread(fontBuffer, fontFileSize, 1, fontFile);
	fclose(fontFile);

	stbtt_InitFont(&font, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer, 0));
}

void TextManager::createTextureAtlas()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "Error before Texture Atlas: " << error << std::endl;
	}

	unsigned char* bitmap = new unsigned char[atlasWidth * atlasHeight];
	stbtt_BakeFontBitmap(fontBuffer, 0, m_fontSize, bitmap, atlasWidth, atlasHeight, 32, 96, &charData[0]);

	glGenTextures(1, &textureAtlas);
	glBindTexture(GL_TEXTURE_2D, textureAtlas);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "glTexImage2D error: " << error << std::endl;
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "glGenerateMipmap error: " << error << std::endl;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "glTexParameteri error: " << error << std::endl;
	}

	delete[] bitmap;
}

void TextManager::init(std::shared_ptr<WindowData> w)
{
	m_windowData = w;
	setupBuffers(2);
	populateVAO();
}

void TextManager::populateVAO()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

std::vector<std::string> TextManager::splitTextIntoLines(const std::string& text)
{
	std::vector<std::string> lines;
	std::istringstream stream{text};
	std::string line;
	while (std::getline(stream, line))
	{
		lines.push_back(line);
	}
	return lines;
}

void TextManager::calculateFullTextSize(const std::vector<std::string> lines)
{
	m_textMetrics.fullTextWidth = 0.0f;
	m_textMetrics.maxCharHeight = 0.0f;

	for (const auto& line : lines)
	{
		float lineWidth = getLineWidth(line);
		if (lineWidth > m_textMetrics.fullTextWidth)
		{
			m_textMetrics.fullTextWidth = lineWidth;
		}
		float lineHeight = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		for (char c : line) {
			if (c >= 32 && c < 128) {
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(&charData[0], atlasWidth, atlasHeight, c - 32, &x, &y, &q, 1);
				float charHeight = q.y1 - q.y0;
				lineHeight = std::max(lineHeight, charHeight);
			}
		}
		m_textMetrics.maxCharHeight = std::max(m_textMetrics.maxCharHeight, lineHeight);
	}
	m_textMetrics.fullTextHeight = lines.size() * m_textMetrics.maxCharHeight;
}

float TextManager::getLineWidth(const std::string line)
{
	float x = 0.0f;
	float y = 0.0f;
	float startX = x;
	stbtt_aligned_quad q;

	for (char c : line) {
		if (c >= 32 && c < 128) {
			stbtt_GetBakedQuad(&charData[0], atlasWidth, atlasHeight, c - 32, &x, &y, &q, 1);
		}
	}
	
	float lineWidth = x - startX;
	return lineWidth;
}

void TextManager::initGeometry()
{
	// if (m_text.empty())
	// {
	// 	return;
	// }

	m_vertices.clear();

	auto lines = splitTextIntoLines(m_text);
	calculateFullTextSize(lines);
	glm::vec4 sizeWorld{m_modelMatrix.m_matrix * glm::vec4{m_textMetrics.fullTextWidth, m_textMetrics.fullTextHeight, 0.0f, 1.0f}};
	setBoundingBox(glm::vec3{0.0f, 0.0f, -0.025f}, glm::vec3{sizeWorld.x, sizeWorld.y, 0.025f});
	float x{}, y{};
	float lineHeight = m_textMetrics.maxCharHeight + 5.0f;
	float startX{};

	for (const auto& line : lines)
	{
		switch (m_justification)
		{
			case TextJustification::Left:
				startX = 0.0f;
				break;
			case TextJustification::Center:
				startX = (m_textMetrics.fullTextWidth - getLineWidth(line)) / 2.0f;
				break;
			case TextJustification::Right:
				startX = m_textMetrics.fullTextWidth - getLineWidth(line);
				break;
			default:
				startX = 0.0f;
				break;
		}
		x = startX;
		for (char c : line)
		{
			if (c >= 32 && c < 128)
			{
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(&charData[0], atlasWidth, atlasHeight, c - 32, &x, &y, &q, 1);

				float x0, x1, y0, y1;

				// Calculate texture & coordinates based on anchor point
				switch (m_anchor)
				{
					case TextAnchor::TopLeft:
						x0 = q.x0;
						x1 = q.x1;
						y0 = q.y0 + m_textMetrics.maxCharHeight;
						y1 = q.y1 + m_textMetrics.maxCharHeight;
						break;
					case TextAnchor::CenterLeft:
						x0 = q.x0;
						x1 = q.x1;
						y0 = q.y0;
						y1 = q.y1;
						break;
					case TextAnchor::BottomLeft:
						x0 = q.x0;
						x1 = q.x1;
						y0 = q.y0 - lineHeight * (lines.size()-1);
						y1 = q.y1 - lineHeight * (lines.size()-1);
						break;
					case TextAnchor::TopCenter:
						x0 = q.x0 - (m_textMetrics.fullTextWidth / 2.0f);
						x1 = q.x1 - (m_textMetrics.fullTextWidth / 2.0f);
						y0 = q.y0 + (m_textMetrics.maxCharHeight / 2.0f);
						y1 = q.y1 + (m_textMetrics.maxCharHeight / 2.0f);
						break;
					case TextAnchor::Center:
						x0 = q.x0 - (m_textMetrics.fullTextWidth / 2.0f);
						x1 = q.x1 - (m_textMetrics.fullTextWidth / 2.0f);
						y0 = q.y0 + (m_textMetrics.maxCharHeight / 2.0f);
						y1 = q.y1 + (m_textMetrics.maxCharHeight / 2.0f);
						break;
					case TextAnchor::BottomCenter:
						x0 = q.x0 - (m_textMetrics.fullTextWidth / 2.0f);
						x1 = q.x1 - (m_textMetrics.fullTextWidth / 2.0f);
						y0 = q.y0 + (m_textMetrics.maxCharHeight / 2.0f);
						y1 = q.y1 + (m_textMetrics.maxCharHeight / 2.0f);
						break;
					case TextAnchor::TopRight:
						x0 = q.x0 - m_textMetrics.fullTextWidth;
						x1 = q.x1 - m_textMetrics.fullTextWidth;
						y0 = q.y0 + m_textMetrics.maxCharHeight;
						y1 = q.y1 + m_textMetrics.maxCharHeight;
						break;
					case TextAnchor::CenterRight:
						x0 = q.x0 - m_textMetrics.fullTextWidth;
						x1 = q.x1 - m_textMetrics.fullTextWidth;
						y0 = q.y0;
						y1 = q.y1;
						break;
					case TextAnchor::BottomRight:
						x0 = q.x0 - m_textMetrics.fullTextWidth;
						x1 = q.x1 - m_textMetrics.fullTextWidth;
						y0 = q.y0;
						y1 = q.y1;
						break;
					default:
						x0 = q.x0 - (m_textMetrics.fullTextWidth / 2.0f);
						x1 = q.x1 - (m_textMetrics.fullTextWidth / 2.0f);
						y0 = q.y0;
						y1 = q.y1;
						break;
				}

				float t0 = q.t0;
				float t1 = q.t1;

				float s0 = q.s0;
				float s1 = q.s1;

				if (m_viewMode == TextViewingMode::Perspective)
				{
					m_modelMatrix.m_scaleFactor = 0.01f;
					m_modelMatrix.updateAll();
					y0=-y0;
					y1=-y1;
				}

				m_vertices.insert(m_vertices.end(), {
					x0, y0, s0, t0,
					x1, y0, s1, t0,
					x1, y1, s1, t1,

					x0, y0, s0, t0,
					x1, y1, s1, t1,
					x0, y1, s0, t1
				});
			}
		}
		y += lineHeight;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_DYNAMIC_DRAW);
}

void TextManager::generateText(std::string text)
{
	m_text = text;
	initGeometry();
}

void TextManager::setUniforms()
{
	glm::mat4 mvpMatrix {1.0f};
	if (m_viewMode == TextViewingMode::Orthographic)
	{
		mvpMatrix = m_windowData->m_orthographic * glm::mat4{1.0f} * m_modelMatrix.m_matrix;
	}
	else if (m_viewMode == TextViewingMode::Perspective)
	{
		mvpMatrix = m_windowData->m_perspective * m_windowData->m_view * m_modelMatrix.m_matrix;
	}
	m_shader->setMat4("mvpMatrix", mvpMatrix);
}

void TextManager::renderSpecifics()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureAtlas);

	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 4);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextManager::setJustification(TextJustification j)
{
	m_justification = j;
	generateText(m_text);
}

void TextManager::setAnchor(TextAnchor a)
{
	m_anchor = a;
	generateText(m_text);
}

TextManager::~TextManager()
{
	delete[] fontBuffer;
}