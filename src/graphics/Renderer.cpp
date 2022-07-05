#include "Renderer.hpp"

#include "util/Log.h"

#include <glad/glad.h>
#include <glm/ext.hpp>

static constexpr size_t k_MaxVertices = 8 * 1024;

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

static glm::mat4 s_ViewProj;
static BatchRendererData s_RendererData;

void Renderer::InitRenderer()
{
	const char *vertexShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) in vec3 a_Position;\n"
		"layout (location = 1) in vec3 a_Normal;\n"
		"layout (location = 2) in vec4 a_Colour;\n"
		"out vec3 v_Normal;\n"
		"out vec4 v_Colour;\n"
		"uniform mat4 u_ViewProj;\n"
		"void main()\n"
		"{\n"
		"	v_Normal = a_Normal;\n"
		"	v_Colour = a_Colour;\n"
		"	gl_Position = u_ViewProj * vec4(a_Position, 1.0);\n"
		"}\n";

	const char *fragmentShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) out vec4 o_Colour;\n"
		"in vec3 v_Normal;\n"
		"in vec4 v_Colour;\n"
		"void main()\n"
		"{\n"
		"	o_Colour = v_Colour;\n"
		"	o_Colour.rgb *= max(0.0, dot(vec3(0.0, 0.0, -1.0), v_Normal));\n"
		"}\n";

	int success;
	char infoLog[512];

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		LOG("Vertex shader failed to compile! %s", infoLog);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		LOG("Fragnent shader failed to compile! %s", infoLog);
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
	glBindVertexArray(s_RendererData.Vao);
	glDrawArrays(GL_TRIANGLES, 0, s_RendererData.VerticesCount);
	glBindVertexArray(0);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	LOG("Renderer was successfully initialized !");
}

void Renderer::Terminate()
{
	CleanupRenderer();

	LOG("Renderer was successfully terminated !");
}

void Renderer::Clear()
{
	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::BeginScene(const glm::mat4 &viewProj)
{
	s_ViewProj = viewProj;

	GLint loc;

	glUseProgram(s_RendererData.Program);
	loc = glGetUniformLocation(s_RendererData.Program, "u_ViewProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(s_ViewProj));
	glUseProgram(0);
}

void Renderer::EndScene()
{
	// TODO: Sorting etc.

	FlushScene();
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