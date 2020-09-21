#include <glad/glad.h>

#include "Shader.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	id = glCreateProgram();

	compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
}

Shader::~Shader()
{
	for(auto shader : shaders)
	{
		glDeleteShader(shader);
	}
}

bool Shader::compileShader(std::string shaderPath, unsigned int type)
{
	unsigned int shader = glCreateShader(type);
	std::string shaderSource = readShaderFile(shaderPath); 
	const char* sSource = shaderSource.c_str();
	glShaderSource(shader, 1, &sSource, nullptr);
	glCompileShader(shader);

	// Error checking
	char infoLog[1024];
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	
	if(!success)
	{
		std::string shaderType;
		switch (type)
		{
			case GL_VERTEX_SHADER:
				shaderType = "VERTEX"; break;
			case GL_FRAGMENT_SHADER:
				shaderType = "FRAGMENT"; break;
			case GL_TESS_CONTROL_SHADER:
				shaderType = "TESS CONTROL"; break;
			case GL_TESS_EVALUATION_SHADER:
				shaderType = "TESS EVALUATION"; break;
		}
		glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
		std::cerr << shaderType << " SHADER COMPILATION FAILED\n" <<
			shaderPath << "\n" << infoLog << std::endl;
	}
   	else
	{
		glAttachShader(id, shader);
		shaders.push_back(shader);
	}
	return success;
}

bool Shader::link()
{
	glLinkProgram(id);

	char infoLog[1024];
	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 1024, nullptr, infoLog);
		std::cerr << "PROGRAM LINKAGE FAILED\n" << infoLog << std::endl;
		
	}
	return success;
}

std::string Shader::readShaderFile(std::string shaderPath)
{
	std::ifstream in(shaderPath);
	std::string buffer = [&in] {
		std::ostringstream ss{};
		ss << in.rdbuf();
		return ss.str();
	}();
	in.close();
	return buffer;
}

void Shader::use()
{
	glUseProgram(id);
}

unsigned int Shader::getId() const
{
	return id;
}
