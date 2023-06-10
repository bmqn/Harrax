#pragma once

#include "app/Window.hpp"

#include <memory>

struct Config
{
	std::string Name;
};

class App
{
public:
	static App *Get()
	{
		static App app;
		return &app;
	}

public:
	void Run(const Config &config);
	Window &GetWindow() { return *m_Window; }

	void OnEvent(Event &e);

private:
	App() = default;

private:
	std::unique_ptr<Window> m_Window = nullptr;
	bool m_IsRunning = false;
};