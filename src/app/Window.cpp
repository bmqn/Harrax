#include "Window.hpp"

#include "util/Log.h"

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

bool Window::Create(const WindowProps &props, std::function<void(Event &e)> callback)
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
	m_Data->Callback = callback;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	m_Data->WindowHandle = glfwCreateWindow(
		m_Data->Width, m_Data->Height, m_Data->Title.c_str(),
		NULL, NULL
	);

	if (!m_Data->WindowHandle)
	{
		ASSERT(false, "Failed to create GLFW window !");
		m_Data.reset(nullptr);
		return false;
	}

	glfwMakeContextCurrent(m_Data->WindowHandle);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		ASSERT(false, "Failed to load OpenGL using GLAD !");
		glfwDestroyWindow(m_Data->WindowHandle);
		m_Data.reset(nullptr);
		return false;
	}

	glfwSetWindowUserPointer(m_Data->WindowHandle, m_Data.get());

	glfwSetWindowCloseCallback(m_Data->WindowHandle, [](GLFWwindow *window) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
		WindowCloseEvent e;
		data.Callback(e);
	});

	glfwSetWindowSizeCallback(m_Data->WindowHandle, [](GLFWwindow *window, int width, int height) {
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		WindowResizeEvent e(width, height);
		data.Callback(e);
	});

	glfwSetKeyCallback(m_Data->WindowHandle, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		switch (action)
		{
			case GLFW_RELEASE:
			{
				KeyReleasedEvent e(key, mods);
				data.Callback(e);
				break;
			}
			case GLFW_PRESS:
			{
				KeyPressedEvent e(key, mods);
				data.Callback(e);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyRepeatEvent e(key, mods);
				data.Callback(e);
				break;
			}
		};
	});

	glfwSetMouseButtonCallback(m_Data->WindowHandle, [](GLFWwindow *window, int button, int action, int mods) {
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		switch (action)
		{
			case GLFW_RELEASE:
			{
				MouseReleasedEvent e(button, mods);
				data.Callback(e);
				break;
			}
			case GLFW_PRESS:
			{
				MousePressedEvent e(button, mods);
				data.Callback(e);
				break;
			}
		};
	});

	glfwSetCursorPosCallback(m_Data->WindowHandle, [](GLFWwindow *window, double xpos, double ypos) {
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MouseMovedEvent e(xpos, ypos);
		data.Callback(e);
	});

	glfwSetScrollCallback(m_Data->WindowHandle, [](GLFWwindow *window, double xoffset, double yoffset) {
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MouseScrolledEvent e(xoffset, yoffset);
		data.Callback(e);
	});

	return true;
}

void Window::Destroy()
{
	if (!m_Data)
	{
		ASSERT(false, "Trying to destory Window which has not been created !");
		return;
	}

	glfwDestroyWindow(m_Data->WindowHandle);
	m_Data.reset(nullptr);
}

bool Window::ShouldClose()
{
	if (!m_Data)
	{
		return true;
	}

	return glfwWindowShouldClose(m_Data->WindowHandle);
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

	glfwSwapBuffers(m_Data->WindowHandle);
}