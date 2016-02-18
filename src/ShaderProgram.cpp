#include "ShaderProgram.h"
#include <iostream>
#include <fstream>

#ifdef __unix__
#include <GL/glew.h>

#elif defined(_WIN32)
#include "..\thirdparties\glew-1.13.0\include\GL\glew.h"
#endif

unsigned int ShaderProgram::LoadShader(unsigned int shaderType, const char* shaderFile)
{
	std::ifstream ifs;
	ifs.open(shaderFile, std::ifstream::in);

	if (!ifs)
	{
		std::cerr << "Cannot open shader file: \"" << shaderFile << "\"" << std::endl;
		return 0;
	}

	std::string source(std::istreambuf_iterator<char>(ifs), (std::istreambuf_iterator<char>()));
	ifs.close();

	GLuint shader = glCreateShader(shaderType);

	const GLchar* sources[] = { source.c_str() };
	glShaderSource(shader, 1, sources, NULL);
	glCompileShader(shader);
	//	std::cout << shader << std::endl;

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus != GL_TRUE)
	{
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* infoLog = new GLchar[logLength];
		glGetShaderInfoLog(shader, logLength, NULL, infoLog);

		std::cerr << infoLog << std::endl;
		delete infoLog;
		return 0;
	}

	return shader;
}

ShaderProgram::ShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
	vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderSource);
	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

	if (linkStatus != GL_TRUE)
	{
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* infoLog = new GLchar[logLength];
		glGetProgramInfoLog(program, logLength, NULL, infoLog);

		std::cerr << infoLog << std::endl;

		delete infoLog;
		program = 0;
	}
}

ShaderProgram::~ShaderProgram() {
	// TODO Auto-generated destructor stub
}

void
ShaderProgram::UseShader() const
{
	glUseProgram(program);
}


unsigned int 
ShaderProgram::GetProgramId() const
{
	return program;
}

void ShaderProgram::DeleteShader()
{
	glUseProgram(0);
	return;
}
