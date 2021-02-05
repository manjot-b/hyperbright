#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

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

Shader::~Shader() {}

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

	// No longer need individual shaders.
	for(auto shader : shaders)
	{
		glDeleteShader(shader);
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

void Shader::use() const
{
	glUseProgram(id);
}

unsigned int Shader::getId() const
{
	return id;
}

void Shader::setUniform1i(const char *uniform, int value) const
{
	GLint uniformLocation = glGetUniformLocation(id, uniform);
	glUniform1i(uniformLocation, value);
	logUniformError(uniformLocation, uniform);
}

void Shader::setUniform1f(const char *uniform, float value) const
{
	GLint uniformLocation = glGetUniformLocation(id, uniform);
	glUniform1f(uniformLocation, value);
	logUniformError(uniformLocation, uniform);
}

void Shader::setUniformMatrix4fv(const char *uniform, const glm::mat4 &matrix) const
{
	GLint uniformLocation = glGetUniformLocation(id, uniform);
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
	logUniformError(uniformLocation, uniform);
}

void Shader::setUniform3fv(const char *uniform, const glm::vec3 &vec) const
{
	GLint uniformLocation = glGetUniformLocation(id, uniform);
	glUniform3fv(uniformLocation, 1, glm::value_ptr(vec));
	logUniformError(uniformLocation, uniform);
}

void Shader::setUniform4fv(const char *uniform, const glm::vec4 &vec) const
{
	GLint uniformLocation = glGetUniformLocation(id, uniform);
	glUniform4fv(uniformLocation, 1, glm::value_ptr(vec));
	logUniformError(uniformLocation, uniform);
}

void Shader::logUniformError(GLint uniformLocation, const char *uniform) const
{
	if (uniformLocation == -1)
	{
		std::cout << "ERROR: Could not find uniform " << uniform << std::endl;
	}
	GLenum status = glGetError();
	while (status != GL_NO_ERROR)
	{
		if (status == GL_INVALID_OPERATION)
		{
			std::cout << "ERROR GL_INVALID_OPERATION: Setting " << uniform << std::endl;
		}
		status = glGetError();
	}
}
