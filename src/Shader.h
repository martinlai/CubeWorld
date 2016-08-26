/* By Martin Lai, last edited August 26, 2016.
All rights reserved. */

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>


enum class SHADER { VERTEX, FRAGMENT };

class Shader
{
public:
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void Activate();

private:
	const GLchar* vsCode;
	const GLchar* fsCode;

	std::string vsStr;
	std::string fsStr;

	std::vector<GLuint> shaders;
	GLuint vertexShader;
	GLuint fragmentShader;

	GLint success;

	void ReadShaderSource(SHADER shader_type, const GLchar* filePath, std::string &tempString, const GLchar* &destString);
	void CompileShader(SHADER shader_type, const GLchar* &codeString, GLuint &shader);
	void CreateShaderProgram();
};


void Shader::ReadShaderSource(SHADER shader_type, const GLchar* filePath, std::string &tempString, const GLchar* &destString) {
	std::ifstream shaderFile(filePath);

	if (shaderFile) {
		std::stringstream fileStream;
		fileStream << shaderFile.rdbuf();
		// Convert to C string
		tempString = fileStream.str();
		destString = tempString.c_str();

	}
	else {
		std::cout << "Error: Unable to open " << filePath << " for reading" << std::endl;
	}

}

void Shader::CompileShader(SHADER shader_type, const GLchar* &codeString, GLuint &shader) {
	std::string shader_name;

	switch (shader_type) {
		case SHADER::VERTEX:
			shader = glCreateShader(GL_VERTEX_SHADER);
			shader_name = "VERTEX";
			break;
		case SHADER::FRAGMENT:
			shader = glCreateShader(GL_FRAGMENT_SHADER);
			shader_name = "FRAGMENT";
			break;
	}

	glShaderSource(shader, 1, &codeString, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {

		// Get error message and store it into a vector
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		// Print error message
		std::cout << "Error compiling " <<  shader_name << " shader:" << std::string(infoLog.begin(), infoLog.end()) << std::endl;
		glDeleteShader(shader);
	}
	shaders.push_back(shader);
}

void Shader::CreateShaderProgram() {
	Program = glCreateProgram();
	for (GLuint shader : shaders) {
		glAttachShader(Program, shader);
	}

	glLinkProgram(Program);
	glGetProgramiv(Program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {

		// Get error message and store it into a vector
		GLint maxLength = 0;
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(Program, maxLength, NULL, &infoLog[0]);

		// Print error message
		std::cout << "Error linking the shader program: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;
	}


	for (GLuint shader : shaders) {
		glDetachShader(Program, shader);
		glDeleteShader(shader);
	}

}

Shader::Shader(const GLchar* vsFilePath, const GLchar* fsFilePath) {


	ReadShaderSource(SHADER::FRAGMENT, fsFilePath, fsStr, fsCode);
	CompileShader(SHADER::FRAGMENT, fsCode, fragmentShader);

	ReadShaderSource(SHADER::VERTEX, vsFilePath, vsStr, vsCode);
	CompileShader(SHADER::VERTEX, vsCode, vertexShader);

	CreateShaderProgram();
}

void Shader::Activate() { glUseProgram(this->Program); }

#endif
