#pragma once

#include "app/Window.hpp"

#include <memory>

class App
{
public:
	static App *Get()
	{
		static App app;
		return &app;
	}

public:
	void Run();
	Window &GetWindow() { return *m_Window; }

private:
	App() = default;

private:
	std::unique_ptr<Window> m_Window;
};