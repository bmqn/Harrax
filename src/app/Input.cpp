#include "Input.hpp"

#include "util/Log.h"
#include "app/App.hpp"

#include <GLFW/glfw3.h>

bool Input::GetKeyDown(int key)
{
	auto *window = static_cast<GLFWwindow*>(App::Get()->GetWindow().GetWindowHandle());
	int state = glfwGetKey(window, key);
	return state == GLFW_PRESS;
}

void Input::DisableCursor()
{
	auto *window = static_cast<GLFWwindow*>(App::Get()->GetWindow().GetWindowHandle());
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::EnableCursor()
{
	auto *window = static_cast<GLFWwindow*>(App::Get()->GetWindow().GetWindowHandle());
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Input::DisableRawMouseInput()
{
	auto *window = static_cast<GLFWwindow*>(App::Get()->GetWindow().GetWindowHandle());
	if (glfwRawMouseMotionSupported())
	{
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}
	else
	{
		LOG("Could not disable raw mouse input - it is not supported.");
	}
}

void Input::EnableRawMouseInput()
{
	auto *window = static_cast<GLFWwindow*>(App::Get()->GetWindow().GetWindowHandle());
	if (glfwRawMouseMotionSupported())
	{
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else
	{
		LOG("Could not enable raw mouse input - it is not supported.");
	}
}

glm::vec2 Input::GetMousePosition()
{
	auto *window = static_cast<GLFWwindow*>(App::Get()->GetWindow().GetWindowHandle());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return { static_cast<float>(xpos), static_cast<float>(ypos) };
}