#include "../../Renderer.hpp"

#include "util/Log.h"

#include <glm/ext.hpp>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>

struct RendererData
{
	GLuint Program;

	RendererData()
		: Program(0)
	{
	}
};

static RendererData s_RendererData;
static Camera s_Camera;

void InitRenderer()
{
	const GLchar *vertexSrc =
		"precision mediump float; \n"
		"attribute vec3 a_Position; \n"
        "attribute vec3 a_Normal; \n"
        "attribute vec4 a_Colour; \n"
        "varying vec3 v_Normal; \n"
        "varying vec4 v_Colour; \n"
		"uniform mat4 u_View; \n"
		"uniform mat4 u_Proj; \n"
		"void main() \n"
		"{ \n"
        "   v_Normal = a_Normal; \n"
        "   v_Colour = a_Colour; \n"
		"   gl_Position = u_Proj * u_View * vec4(a_Position, 1.0); \n"
		"} \n";

	const GLchar *fragmentSrc =
		"precision mediump float; \n"
        "varying vec3 v_Normal; \n"
        "varying vec4 v_Colour; \n"
		"vec3 ambientColour = vec3(0.2, 0.4, 0.4); \n"
		"float ambientStrength = 1.0; \n"
		"vec3 lightColour = vec3(0.6, 0.8, 0.4); \n"
		"vec3 lightDirection = vec3(0.0, 1.0, 0.0); \n"
		"void main() \n"
		"{ \n"
		"	vec3 ambient = ambientColour * ambientStrength; \n"
		"	vec3 diffuse = lightColour * max(0.5, dot(normalize(-lightDirection), v_Normal)); \n"
		"   gl_FragColor = vec4(ambient + diffuse, 1.0) * v_Colour; \n"
		"} \n";

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

	glBindAttribLocation(s_RendererData.Program, 0, "a_Position");
    glBindAttribLocation(s_RendererData.Program, 1, "a_Normal");
    glBindAttribLocation(s_RendererData.Program, 2, "a_Colour");

	glLinkProgram(s_RendererData.Program);

	glDetachShader(s_RendererData.Program, vertexShader);
	glDetachShader(s_RendererData.Program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
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

}

void Renderer::BeginScene(const Camera &camera)
{
	s_Camera = camera;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewMatrix = camera.GetViewMatrix();
	glm::mat4 profMatrix = camera.GetProjMatrix();

	GLint loc;
	glUseProgram(s_RendererData.Program);
	loc = glGetUniformLocation(s_RendererData.Program, "u_View");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	loc = glGetUniformLocation(s_RendererData.Program, "u_Proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(profMatrix));
	glUseProgram(0);
}

void Renderer::EndScene()
{
}

const Camera &Renderer::GetCamera()
{
	return s_Camera;
}

void Renderer::SubmitTriangle(const std::array<glm::vec3, 3> &vertices, glm::vec4 colour)
{
	glm::vec3 normal = glm::normalize(glm::cross(
			vertices[1] - vertices[0],
			vertices[2] - vertices[0]
	));

	GLfloat data[] = {
			vertices[0].x, vertices[0].y, vertices[0].z, normal.x, normal.y, normal.z, colour.r, colour.g, colour.b, colour.a,
			vertices[1].x, vertices[1].y, vertices[1].z, normal.x, normal.y, normal.z, colour.r, colour.g, colour.b, colour.a,
			vertices[2].x, vertices[2].y, vertices[2].z, normal.x, normal.y, normal.z, colour.r, colour.g, colour.b, colour.a
	};

	glUseProgram(s_RendererData.Program);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid *)(&data[0]));
	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid *)(&data[3]));
	glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid *)(&data[6]));
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(0);
}


void Renderer::SubmitQuad(const std::array<glm::vec3, 4> &vertices, glm::vec4 colour)
{
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