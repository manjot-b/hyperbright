#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

/*
 * Compiles multiples shaders and links them into
 * a shader program
 */
class Shader
{
	public:
		Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
		~Shader();
		unsigned int getId() const;
		bool compileShader(std::string shaderPath, unsigned int type);
		bool link();
		void use() const;
		void setUniform1i(const char *uniform, int value) const;
		void setUniform1f(const char *uniform, float value) const;
		void setUniformMatrix4fv(const char *uniform, const glm::mat4 &matrix) const;
		void setUniform3fv(const char *uniform, const glm::vec3 &vec) const;
		void setUniform4fv(const char *uniform, const glm::vec4 &vec) const;

	private:
		unsigned int id;
		std::vector<unsigned int> shaders;
		std::string readShaderFile(std::string shaderPath);
		void logUniformError(GLint uniformLocation, const char *uniform) const;
};
