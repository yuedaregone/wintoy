#include "shader.h"
#include <stdio.h>
#include <iostream>
#include <string.h>

GLuint loadShaders(const char* vertexStr, const char* fragmentStr)
{
	//Create the shaders
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	GLint iResult = GL_FALSE;
	int InfoLogLength = 0;

	//Compile Vertex Shader
	glShaderSource(vertexShaderId, 1, &vertexStr, NULL);
	glCompileShader(vertexShaderId);

	//Check Vertex Shader
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &iResult);
	glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	GLchar* infoBuff = new GLchar[InfoLogLength+1];
	memset(infoBuff, 0, InfoLogLength + 1);
	glGetShaderInfoLog(vertexShaderId, InfoLogLength, NULL, infoBuff);
	std::cout<<infoBuff;
	delete[] infoBuff;

	//Compile Fragment Shader
	glShaderSource(fragmentShaderId, 1, &fragmentStr, NULL);
	glCompileShader(fragmentShaderId);

	//Check Fragment Shader
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &iResult);
	glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	GLchar* infoBuff1 = new GLchar[InfoLogLength + 1];
	memset(infoBuff1, 0, InfoLogLength + 1);
	glGetShaderInfoLog(fragmentShaderId, InfoLogLength, NULL, infoBuff1);
	std::cout << infoBuff1;
	delete[] infoBuff1;

	//Link the program
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	//Check the program
	glGetProgramiv(programId, GL_LINK_STATUS, &iResult);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	GLchar* infoBuff2 = new GLchar[InfoLogLength + 1];
	memset(infoBuff2, 0, InfoLogLength + 1);
	glGetProgramInfoLog(programId, InfoLogLength, NULL, infoBuff2);
	std::cout<<infoBuff2;
	delete[] infoBuff2;

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	return programId;
}