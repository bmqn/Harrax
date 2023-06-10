#pragma once

#include "Event.hpp"

#include <cstdint>
#include <string>
#include <memory>
#include <functional>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct WindowProps
{
	uint32_t Width, Height;
	std::string Title;
};

struct WindowData
{
	uint32_t Width, Height;
	std::string Title;
	std::function<void(Event &e)> Callback;

	GLFWwindow *WindowHandle;
};

class Window
{
public:
	static void Init();
	static void Terminate();

public:
	Window() = default;
	
	bool Create(const WindowProps& props, std::function<void(Event &e)> callback);
	void Destroy();

	bool ShouldClose();

	void PollEvents();
	void SwapBuffers();

	GLFWwindow *GetWindowHandle() { return m_Data->WindowHandle; }

private:
	std::unique_ptr<WindowData> m_Data;
};