#include "framework.h"
#include "GL\glew.h"
#include <time.h>
#include "ftime.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "shader.h"
#include "tools.h"

const char* verStr = "\
#version 120													\n\
attribute vec3 vertexPosition_modelspace;						\n\
attribute vec4 vertextColor;									\n\
attribute vec2 uv;												\n\
varying vec4 vColor;											\n\
varying vec2 fragCoord;											\n\
void main(){													\n\
	gl_Position = vec4(vertexPosition_modelspace, 1.0);			\n\
	fragCoord =	uv;												\n\
	//fragCoord.y = 1 - fragCoord.y;							\n\
	vColor = vertextColor;										\n\
}																\n\
";

const char* startStr = "\
#version 120											\n\
varying vec4 vColor;									\n\
varying vec2 fragCoord;									\n\
uniform vec2 iResolution;								\n\
uniform vec2 iMouse;								\n\
uniform float iTime; \n";
const char* endStr = "\
void main() {												\n\
	vec2 p = fragCoord;										\n\
	p = p * iResolution;									\n\
	vec4 fragColor = vec4(fragCoord.x,fragCoord.y,0,1);		\n\
	mainImage(fragColor, p);								\n\
	gl_FragColor = fragColor;								\n\
}															\n\
";
int s_width = 0;
int s_height = 0;

int GetFPS()
{
	return 30;
}

static void InitOpenGL(int width, int height)
{
	s_width = width;
	s_height = height;

	//glEnable(GL_ALPHA_TEST);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	glClearColor(0, 0, 0, 0);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

std::string GetShaderInFile(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
		return "";

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* buf = new char[size + 1];
	memset(buf, 0, size);
	fread(buf, 1, size, fp);

	std::string shader = buf;

	delete[] buf;
	return shader;
}

GLuint vertex = 0;
GLuint color = 0;
GLuint uv = 0;
GLuint shader = 0;
GLuint vertexColorID = 0;
GLuint vertexPosID = 0;
GLuint uvID = 0;
GLuint uniformTimeID = 0;
GLuint resolutionID = 0;

std::string curShader;

void ReloadShaderData()
{	
	std::string path = GetCurrentPath();
	std::vector<std::string> filenames;
	LoadAllFileNames(path.c_str(), filenames, false, ".toy");

	if (curShader.empty())
	{
		curShader = filenames[0];
	}
	else
	{
		std::vector<std::string>::iterator it = std::find(filenames.begin(), filenames.end(), curShader);
		if (it == filenames.end())
		{
			curShader = filenames[0];
		}
		else
		{
			curShader = *(++it);
		}
	}

	std::string fragShader = startStr;
	fragShader += GetShaderInFile(curShader.c_str());
	fragShader += endStr;

	shader = loadShaders(verStr, fragShader.c_str());

	vertexPosID = glGetAttribLocation(shader, "vertexPosition_modelspace");
	uvID = glGetAttribLocation(shader, "uv");
	vertexColorID = glGetAttribLocation(shader, "vertextColor");
	uniformTimeID = glGetUniformLocation(shader, "iTime");
	resolutionID = glGetUniformLocation(shader, "iResolution");
}

void InitBuffData()
{
	glGenBuffers(1, &vertex);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	static float v_p[18] =
	{
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(v_p), v_p, GL_STATIC_DRAW);

	glGenBuffers(1, &uv);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	static float v_uv[12] =
	{
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(v_uv), v_uv, GL_STATIC_DRAW);

	glGenBuffers(1, &color);
	glBindBuffer(GL_ARRAY_BUFFER, color);
	static float v_color[24] =
	{
		1.0f, 0.0f, 0.0f, 0.2f,
		0.0f, 1.0f, 0.0f, 0.4f,
		0.0f, 0.0f, 1.0f, 0.6f,

		1.0f, 0.0f, 0.0f, 0.2f,
		0.0f, 1.0f, 0.0f, 0.4f,
		0.0f, 0.0f, 1.0f, 0.6f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(v_color), v_color, GL_STATIC_DRAW);	
}


static void DestroyOpenGL()
{

}

static void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/*
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_TRIANGLES);

	glColor4f(0.2f, 0.5f, 0.2f, 0.5f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glEnd();

	glBegin(GL_TRIANGLES);

	glColor4f(0.7f, 0.3f, 0.1f, 0.5f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	glEnd();
	*/

	glUseProgram(shader);	
	glUniform1f(uniformTimeID, Time::time());
	glUniform2f(resolutionID, (GLfloat)s_width, (GLfloat)s_height);

	glEnableVertexAttribArray(vertexPosID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glVertexAttribPointer(
		vertexPosID,		// attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		0,					// stride
		(void*)0);			// array buffer offset

	glEnableVertexAttribArray(uvID);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(uvID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(vertexColorID);
	glBindBuffer(GL_ARRAY_BUFFER, color);
	glVertexAttribPointer(vertexColorID, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 1; 3 vertices total -&gt; 1 triangle
	glDisableVertexAttribArray(vertexPosID);
	glDisableVertexAttribArray(vertexColorID);
	
}


void InitFramework(int width, int height, void* window)
{
	InitOpenGL(width, height);	
	InitBuffData();
	ReloadShaderData();
}

void UpdateFramework()
{
	Render();
}

void EndFramework()
{
	DestroyOpenGL();
}
