#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

#include "shader.h"

class ShaderManager
{
public:
	static ShaderManager& getInstance()
	{
		static ShaderManager instance;
		return instance;
	}

	std::shared_ptr<Shader> loadNewShader(const std::string& s, const char* vs, const char* fs);
	std::shared_ptr<Shader> getShader(const std::string& s);

	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

private:
	ShaderManager()
	{}

	std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders{};
};

#endif
