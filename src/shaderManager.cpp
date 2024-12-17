#include "../include/shaderManager.h"

std::shared_ptr<Shader> ShaderManager::loadNewShader(const std::string& s, const char* vs, const char* fs)
{
	if (m_shaders.find(s) == m_shaders.end())
	{
		m_shaders[s] = std::make_shared<Shader>(vs, fs);
	}
	else
	{
		std::cout << "Shader already exists under identifier " << s << '\n';
	}
	return m_shaders[s];
}

std::shared_ptr<Shader> ShaderManager::getShader(const std::string& s)
{
	if (m_shaders.find(s) == m_shaders.end())
	{
		std::cout << "Shader with handle " << s << " does not exist.\n";
		return nullptr;
	}
	
	return m_shaders[s];
}