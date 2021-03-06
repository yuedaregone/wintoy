#include "framework.h"
#include "GL\glew.h"
#include "ftime.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "shader.h"
#include "tools.h"
#include "timer.h"
#include <time.h>


#ifdef __cplusplus 
extern "C" {
#endif
#include "http.h"
#include "buffer.h"
#include "skt.h"
#ifdef __cplusplus 
}
#endif

static void FetchFileLists();
static void FetchFileContent(const char* filename);


const char* verStr = "\
#version 330													\n\
in vec3 vertexPosition_modelspace;						\n\
in vec4 vertextColor;									\n\
in vec2 uv;												\n\
out vec4 vColor;											\n\
out vec2 fragCoord;											\n\
void main(){													\n\
	gl_Position = vec4(vertexPosition_modelspace, 1.0);			\n\
	fragCoord =	uv;												\n\
	//fragCoord.y = 1 - fragCoord.y;							\n\
	vColor = vertextColor;										\n\
}																\n\
";

const char* startStr = "\
#version 330											\n\
in vec4 vColor;									\n\
in vec2 fragCoord;									\n\
uniform vec2 iResolution;								\n\
uniform vec2 iMouse;								\n\
uniform float iTime;								\n\
out vec4 g_fragColor;									\n";
const char* endStr = "\
void main()										\n\
{												\n\
	vec2 p = fragCoord;										\n\
	p = p * iResolution;									\n\
	vec4 fragColor = vec4(fragCoord.x,fragCoord.y,0.0,1.0);		\n\
	mainImage(fragColor, p);								\n\
	g_fragColor = fragColor;								\n\
}															\n\
";

const char* defaultShader = "\
void mainImage( out vec4 fragColor, in vec2 fragCoord )     \n\
{															\n\
	vec2 uv = fragCoord / iResolution.xy;					\n\
	vec3 col = 0.5 + 0.5*cos(iTime + uv.xyx + vec3(0, 2, 4)); \n\
	fragColor = vec4(col, 1.0);									\n\
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	glClearColor(0, 0, 0, 0);

	glViewport(0, 0, width, height);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
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
GLuint mouseID = 0;

int iMouseX = 0;
int iMouseY = 0;

std::string curShader;

void FindPreShader()
{
	std::string path = GetOrCreateWorkPath();
	std::vector<std::string> filenames;
	LoadAllFileNames(path.c_str(), filenames, false, ".toy");

	if (filenames.empty())
	{
		return;
	}

	if (curShader.empty())
	{
		curShader = filenames[0];
	}
	else
	{
		std::vector<std::string>::iterator it = std::find(filenames.begin(), filenames.end(), curShader);
		if (it == filenames.end() || it == filenames.begin())
		{
			curShader = filenames[filenames.size() - 1];
		}
		else
		{
			curShader = *(--it);
		}
	}
}

void FindNextShader()
{
	std::string path = GetOrCreateWorkPath();
	std::vector<std::string> filenames;
	LoadAllFileNames(path.c_str(), filenames, false, ".toy");

	if (filenames.empty())
	{
		return;
	}

	if (curShader.empty())
	{
		srand((unsigned int)time(NULL));
		int rd = rand() % filenames.size();
		curShader = filenames[rd];
	}
	else
	{
		std::vector<std::string>::iterator it = std::find(filenames.begin(), filenames.end(), curShader);
		if (it == filenames.end() || it + 1 == filenames.end())
		{
			curShader = filenames[0];
		}
		else
		{
			curShader = *(++it);
		}
	}
}

bool IsFindFile(const char* name)
{
	std::string path = GetOrCreateWorkPath();
	std::vector<std::string> filenames;
	LoadAllFileNames(path.c_str(), filenames, false, ".toy");

	if (filenames.empty())
	{
		return false;
	}
	std::vector<std::string>::iterator it = std::find(filenames.begin(), filenames.end(), name);
	if (it == filenames.end())
	{
		return false;
	}
	return true;
}

void LoadShaderData()
{	
	std::string fragShader = startStr;
	std::string content = GetShaderInFile(curShader.c_str());
	if (curShader.empty() || content.empty())
	{
		fragShader += defaultShader;
	}
	else
	{
		fragShader += content;
	}	
	fragShader += endStr;

	shader = loadShaders(verStr, fragShader.c_str());

	//GLenum glerror = glGetError();
	//if (glerror != GL_NO_ERROR)
	//	printf(glErrorStringREGAL(glerror));

	vertexPosID = glGetAttribLocation(shader, "vertexPosition_modelspace");
	uvID = glGetAttribLocation(shader, "uv");
	vertexColorID = glGetAttribLocation(shader, "vertextColor");
	uniformTimeID = glGetUniformLocation(shader, "iTime");
	resolutionID = glGetUniformLocation(shader, "iResolution");
	mouseID = glGetUniformLocation(shader, "iMouse");
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
	glDeleteBuffers(1, &vertex);
	glDeleteBuffers(1, &uv);
	glDeleteBuffers(1, &color);

	glDeleteProgram(shader);
}

static void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader);	
	glUniform1f(uniformTimeID, Time::time());
	glUniform2f(resolutionID, (GLfloat)s_width, (GLfloat)s_height);
	glUniform2f(mouseID, (GLfloat)iMouseX, (GLfloat)iMouseY);

	glEnableVertexAttribArray(vertexPosID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glVertexAttribPointer(vertexPosID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(uvID);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(uvID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(vertexColorID);
	glBindBuffer(GL_ARRAY_BUFFER, color);
	glVertexAttribPointer(vertexColorID, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(vertexPosID);
	glDisableVertexAttribArray(vertexColorID);
	
}

static void OnTimeCheckUpdate()
{
	//FetchFileLists();
}

void InitFramework(int width, int height, void* window)
{
	InitOpenGL(width, height);	
	InitBuffData();
	FindNextShader();
	LoadShaderData();

	char name[16] = { "update" };
	timer_create_timer_day(name, 32400, OnTimeCheckUpdate);
}

void UpdateFramework()
{
	timer_update();
	Render();

	http_client_update();
}

void EndFramework()
{
	DestroyOpenGL();
}

bool TouchBegin(int x, int y)
{
	return true;
}

void TouchMove(int x, int y)
{
	iMouseX = x;
	iMouseY = y;
}

void TouchEnd(int x, int y)
{
}


void TouchNext()
{
	FindNextShader();
	LoadShaderData();
}

void TouchBack()
{
	FindPreShader();
	LoadShaderData();
}

static void OnFetchFileLists(struct http_respond* resp)
{
	int len = buf_size_data(resp->data);
	char* buf = (char*)_alloca(len + 1);
	buf_read_data(resp->data, (int8_t*)buf, len);
	buf[len] = '\0';
	
	std::string path = GetOrCreateWorkPath();
	std::vector<std::string> localFiles;
	LoadAllFileNames(path.c_str(), localFiles, false, ".toy");
	
	std::vector<std::string> filenames;

	std::string str = buf;
	size_t start = 0;
	size_t pos = 0;
	while ((pos = str.find(',', start)) != std::string::npos)
	{
		std::string name = str.substr(start, pos - start);
		if (!name.empty() && std::find(localFiles.begin(), localFiles.end(), name) == localFiles.end())
		{
			filenames.push_back(name);
		}
		start = pos + 1;		
	}

	for (size_t i = 0; i < filenames.size(); ++i)
	{
		FetchFileContent(filenames[i].c_str());		
	}
	
}

static void FetchFileLists()
{
	struct http_client* clt = http_client_get("http://daregone.pythonanywhere.com/wintoy?method=list", NULL);
	clt->cb = OnFetchFileLists;
	http_client_send(clt);
}

static void OnFetchFileContent(struct http_respond* resp)
{
	char name[256] = { 0 };
	http_request_get_path(resp->req, name, 256);

	std::string filename = name;
	if (filename.empty())
		return;

	filename = filename.substr(filename.find("name=") + strlen("name="));

	int len = buf_size_data(resp->data);
	char* buf = (char*)_alloca(len + 1);
	buf_read_data(resp->data, (int8_t*)buf, len);
	buf[len] = '\0';

	std::string path = GetOrCreateWorkPath();
	FILE *fp = fopen((path + "\\" + filename).c_str(), "wb");
	if (fp != NULL)
	{
		fwrite(buf, 1, len, fp);
	}
	fclose(fp);
}


static void FetchFileContent(const char* filename)
{
	std::string reqUrl = "http://daregone.pythonanywhere.com/wintoy?method=file&name=";
	reqUrl += filename;
	struct http_client* clt = http_client_get(reqUrl.c_str(), NULL);
	clt->cb = OnFetchFileContent;
	http_client_send(clt);
}

