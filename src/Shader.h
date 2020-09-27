#pragma once

/*
 * Compiles multiples shaders and links them into
 * a shader program
 */

#include <string>
#include <vector>

class Shader
{
	public:
		Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
		~Shader();
		unsigned int getId() const;
		bool compileShader(std::string shaderPath, unsigned int type);
		bool link();
		void use() const;

	private:
		unsigned int id;
		std::vector<unsigned int> shaders;
		std::string readShaderFile(std::string shaderPath);
};
