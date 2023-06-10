#include "Time.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

double Time::Seconds()
{
	return glfwGetTime();
}