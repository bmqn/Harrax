#include "../../Window.hpp"

#include "util/Log.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

static void GLFWErrorCallback(int error, const char *description)
{
	LOG("GLFW Error (%d), %s", error, description);
}

void Window::Init()
{
	if (!glfwInit())
	{
		ASSERT(false, "GLFW failed to initialize !");
		return;
	}

	glfwSetErrorCallback(GLFWErrorCallback);
}

void Window::Terminate()
{
	glfwTerminate();
}

bool Window::Create(const WindowProps& props)
{
	if (m_Data)
	{
		LOG("Window has already been created !");
		return false;
	}

	m_Data = std::make_unique<WindowData>();
	m_Data->Width = props.Width;
	m_Data->Height = props.Height;
	m_Data->Title = props.Title;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	m_Data->WindowHandle = reinterpret_cast<WindowHandle>(glfwCreateWindow(
		m_Data->Width, m_Data->Height, m_Data->Title.c_str(),
		NULL, NULL
	));

	if (!m_Data->WindowHandle)
	{
		ASSERT(false, "Failed to create GLFW window !");
		m_Data.reset(nullptr);
		return false;
	}

	glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_Data->WindowHandle));
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		ASSERT(false, "Failed to load OpenGL using GLAD !");
		glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Data->WindowHandle));
		m_Data.reset(nullptr);
		return false;
	}

	return true;
}

void Window::Destroy()
{
	if (!m_Data)
	{
		ASSERT(false, "Trying to destory Window which has not been created !");
		return;
	}

	glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Data->WindowHandle));
	m_Data.reset(nullptr);
}

bool Window::ShouldClose()
{
	if (!m_Data)
	{
		return true;
	}

	return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Data->WindowHandle));
}

void Window::PollEvents()
{
	if (!m_Data)
	{
		return;
	}

	glfwPollEvents();
}

void Window::SwapBuffers()
{
	if (!m_Data)
	{
		return;
	}

	glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_Data->WindowHandle));
}