#include "Renderer.hpp"

#include "util/Log.h"
#include "util/File.hpp"

#include <glad/glad.h>
#include <glm/ext.hpp>

static constexpr size_t k_MaxVertices = 64 * 1024;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec4 Colour;
};

struct BatchRendererData
{
	GLuint Program;
	GLuint Vao, Vbo;
	Vertex *BatchDataPtr;
	GLsizei VerticesCount;

	BatchRendererData()
		: Program(0), Vao(0), Vbo(0), BatchDataPtr(nullptr), VerticesCount(0)
	{
	}
};

static BatchRendererData s_RendererData;

void Renderer::InitRenderer()
{
	auto vertexSrcRaw = ReadFile("basic.vertex");
	auto fragmentSrcRaw = ReadFile("basic.fragment");

	auto vertexSrcStr = std::string(vertexSrcRaw.begin(), vertexSrcRaw.end());
	auto fragmentSrcStr = std::string(fragmentSrcRaw.begin(), fragmentSrcRaw.end());

	const char *vertexSrc = vertexSrcStr.c_str();
	const char *fragmentSrc = fragmentSrcStr.c_str();

	int success;
	char infoLog[1024];

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrc, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
		LOG("Vertex shader failed to compile!\n%s", infoLog);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
		LOG("Fragment shader failed to compile!\n%s", infoLog);
	}

	s_RendererData.Program = glCreateProgram();
	glAttachShader(s_RendererData.Program, vertexShader);
	glAttachShader(s_RendererData.Program, fragmentShader);
	glLinkProgram(s_RendererData.Program);

	glDetachShader(s_RendererData.Program, vertexShader);
	glDetachShader(s_RendererData.Program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &s_RendererData.Vao);
	glGenBuffers(1, &s_RendererData.Vbo);

	glBindVertexArray(s_RendererData.Vao);
	glBindBuffer(GL_ARRAY_BUFFER, s_RendererData.Vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * k_MaxVertices, (GLvoid *)0, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(6 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	MapBuffer();
}

void Renderer::CleanupRenderer()
{
	UnmapBuffer();

	glDeleteProgram(s_RendererData.Program);
	glDeleteBuffers(1, &s_RendererData.Vbo);
	glDeleteVertexArrays(1, &s_RendererData.Vao);
}

void Renderer::FlushVertices()
{
	if (s_RendererData.VerticesCount == 0)
	{
		return;
	}

	UnmapBuffer();

	glUseProgram(s_RendererData.Program);
	glDrawArrays(GL_TRIANGLES, 0, s_RendererData.VerticesCount);
	glUseProgram(0);

	s_RendererData.VerticesCount = 0;

	MapBuffer();
}

void Renderer::FlushScene()
{
	FlushVertices();
}

void Renderer::MapBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, s_RendererData.Vbo);
	s_RendererData.BatchDataPtr = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::UnmapBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, s_RendererData.Vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Init()
{
	InitRenderer();

	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::Terminate()
{
	CleanupRenderer();
}

void Renderer::BeginScene(const glm::mat4 &viewProj)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLint loc;
	glUseProgram(s_RendererData.Program);
	loc = glGetUniformLocation(s_RendererData.Program, "u_ViewProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewProj));
	glUseProgram(0);

	glBindVertexArray(s_RendererData.Vao);
}

void Renderer::EndScene()
{
	// TODO: Sorting etc.

	FlushScene();

	glBindVertexArray(0);
}

void Renderer::SubmitTriangle(const std::array<glm::vec3, 3> &vertices, glm::vec4 colour)
{
	if (s_RendererData.VerticesCount + 3 > k_MaxVertices)
	{
		FlushVertices();
	}

	glm::vec3 normal = glm::normalize(glm::cross(
		vertices[1] - vertices[0],
		vertices[2] - vertices[0]
	));

	s_RendererData.BatchDataPtr->Position = vertices[0];
	s_RendererData.BatchDataPtr->Normal = normal;
	s_RendererData.BatchDataPtr->Colour = colour;

	s_RendererData.BatchDataPtr++;

	s_RendererData.BatchDataPtr->Position = vertices[1];
	s_RendererData.BatchDataPtr->Normal = normal;
	s_RendererData.BatchDataPtr->Colour = colour;

	s_RendererData.BatchDataPtr++;

	s_RendererData.BatchDataPtr->Position = vertices[2];
	s_RendererData.BatchDataPtr->Normal = normal;
	s_RendererData.BatchDataPtr->Colour = colour;

	s_RendererData.BatchDataPtr++;

	s_RendererData.VerticesCount += 3;
}

void Renderer::SubmitQuad(const std::array<glm::vec3, 4> &vertices, glm::vec4 colour)
{
	if (s_RendererData.VerticesCount + 4 > k_MaxVertices)
	{
		FlushVertices();
	}

	Renderer::SubmitTriangle(
		{ vertices[0], vertices[1], vertices[2] },
		colour
	);
	Renderer::SubmitTriangle(
		{ vertices[2], vertices[1], vertices[3] },
		colour
	);
}

void Renderer::SubmitCube(const std::array<glm::vec3, 8> &vertices, glm::vec4 colour)
{
	if (s_RendererData.VerticesCount + 3 * 2 * 6 > k_MaxVertices)
	{
		FlushVertices();
	}

	// Front
	Renderer::SubmitTriangle(
		{ vertices[0], vertices[1], vertices[2] },
		colour
	);
	Renderer::SubmitTriangle(
		{ vertices[2], vertices[1], vertices[3] },
		colour
	);

	// Back
	Renderer::SubmitTriangle(
		{ vertices[6], vertices[7], vertices[4] },
		colour
	);
	Renderer::SubmitTriangle(
		{ vertices[4], vertices[7], vertices[5] },
		colour
	);

	// Left
	Renderer::SubmitTriangle(
		{ vertices[4], vertices[5], vertices[0] },
		colour
	);
	Renderer::SubmitTriangle(
		{ vertices[0], vertices[5], vertices[1] },
		colour
	);

	// Right
	Renderer::SubmitTriangle(
		{ vertices[2], vertices[3], vertices[6] },
		colour
	);
	Renderer::SubmitTriangle(
		{ vertices[6], vertices[3], vertices[7] },
		colour
	);

	// Top
	Renderer::SubmitTriangle(
		{ vertices[4], vertices[0], vertices[6] },
		colour
	);
	Renderer::SubmitTriangle(
		{ vertices[6], vertices[0], vertices[2] },
		colour
	);

	// Bottom
	Renderer::SubmitTriangle(
		{ vertices[1], vertices[5], vertices[3] },
		colour
	);
	Renderer::SubmitTriangle(
		{ vertices[3], vertices[5], vertices[7] },
		colour
	);
}