#pragma once

#include "util/Error.hpp"

#include <cstdint>
#include <string>
#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct WindowProps
{
	uint32_t Width;
	uint32_t Height;
	std::string Title;
};

struct WindowData
{
	uint32_t Width;
	uint32_t Height;
	std::string Title;
	GLFWwindow *WindowHandle;
};

class Window
{
public:
	static void Init();
	static void Terminate();

public:
	Window() = default;
	
	bool Create(const WindowProps& props);
	void Destroy();

	bool ShouldClose();

	void PollEvents();
	void SwapBuffers();

private:
	std::unique_ptr<WindowData> m_Data;
};