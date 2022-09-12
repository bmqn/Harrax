#include "../../Input.hpp"

#include "app/App.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

glm::vec2 Input::GetMousePosition()
{
	auto *window = reinterpret_cast<GLFWwindow*>(App::Get()->GetWindow().GetWindowHandle());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	return { static_cast<float>(xpos), static_cast<float>(ypos) };
}