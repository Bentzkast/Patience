#include "main.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

void Shader::Select() const
{
	glUseProgram(this->finalShaderHandle);
}

bool Shader::Set3fUniform(const char* name, glm::vec3 value) const
{
	GLint loc = glGetUniformLocation(this->finalShaderHandle, name);
	// Send the matrix data to the uniform
	if (loc != -1)
	{
		glUniform3f(loc, value.x, value.y, value.z);
		return false;
	}
	return true;
}

bool Shader::SetMatrixUniform(const char* name, const glm::mat4& matrix) const
{
	GLint loc = glGetUniformLocation(this->finalShaderHandle, name);
	// Send the matrix data to the uniform
	if (loc != -1)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
		return false;
	}
	return true;
}

void Shader::Free() const
{
	SDL_Log("Destroying Shader %d", this->finalShaderHandle);
	glDeleteProgram(this->finalShaderHandle);
	glDeleteShader(this->vertexShaderHandle);
	glDeleteShader(this->fragmentShaderHandle);	
}

bool Shader::LoadSingleShader(const char* filepath, GLenum shaderType, Uint32& outHandle)
{
	std::ifstream sourceFile{ filepath };
	if (sourceFile.is_open())
	{
		// Read as a stream
		std::stringstream stringStream;
		stringStream << sourceFile.rdbuf();
		std::string contents = stringStream.str();
		const char* contentsChar = contents.c_str();

		outHandle = glCreateShader(shaderType);
		glShaderSource(outHandle, 1, &(contentsChar), NULL);
		glCompileShader(outHandle);

		// Error checking
		int success;
		char infoLog[512];
		glGetShaderiv(outHandle, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(outHandle, 512, NULL, infoLog);
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to compile shader file : %s", filepath);
			return false;
		}
		return true;
	}
	else
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open shader file: %s", filepath);
		return false;
	}
	return true;
}

bool Shader::LoadFinalShader(const char* vertexPath, const char* fragPath, Shader& outShader)
{
	Uint32 vertexShaderHandle { 0 };
	Uint32 fragmentShaderHandle { 0 };

	if (!LoadSingleShader(vertexPath, GL_VERTEX_SHADER, vertexShaderHandle))
	{
		SDL_Log("Failed to Load Vertex Shader");
		return false;
	}
	if (!LoadSingleShader(fragPath, GL_FRAGMENT_SHADER, fragmentShaderHandle))
	{
		SDL_Log("Failed to Load Frag Shader");
		return false;
	}

	Uint32 shaderHandle = glCreateProgram();
	glAttachShader(shaderHandle, vertexShaderHandle);
	glAttachShader(shaderHandle, fragmentShaderHandle);
	glLinkProgram(shaderHandle);
	
	// Check Link error
	int success;
	char infoLog[512];
	glGetProgramiv(shaderHandle, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(fragmentShaderHandle, 512, NULL, infoLog);
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_glLinkProgram: %s", infoLog);
		return false;
	}
	SDL_Log("Loaded & Compiled %s", vertexPath);
	SDL_Log("Loaded & Compiled %s", fragPath);

	outShader.vertexShaderHandle = vertexShaderHandle;
	outShader.fragmentShaderHandle = fragmentShaderHandle;
	outShader.finalShaderHandle = shaderHandle;
	return true;
}