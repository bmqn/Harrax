#include "Input.hpp"

#include "app/App.hpp"

#include <GLFW/glfw3.h>

glm::vec2 Input::GetMousePosition()
{
	auto *window = static_cast<GLFWwindow*>(App::Get()->GetWindow().GetWindowHandle());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	return { static_cast<float>(xpos), static_cast<float>(ypos) };
}