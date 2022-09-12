#pragma once

#include <cstdint>
#include <string>
#include <memory>

#if HR_TARGET_ANDROID
#include <android/native_window.h>
#endif // HR_TARGET_ANDROID

using WindowHandle = uintptr_t;

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
	WindowHandle WindowHandle;
};

class Window
{
public:
	static void Init();
	static void Terminate();

#if HR_TARGET_ANDROID
	static void SetNativeWindow(ANativeWindow *windowHandle);
	static void SetShouldClose();
#endif // HR_TARGET_ANDROID

public:
	Window() = default;
	
	bool Create(const WindowProps& props);
	void Destroy();

	bool ShouldClose();

	void PollEvents();
	void SwapBuffers();

	WindowHandle GetWindowHandle() { return m_Data->WindowHandle; };
	uint32_t GetWidth() { return m_Data->Width; }
	uint32_t GetHeight() { return m_Data->Height; }

private:
	std::unique_ptr<WindowData> m_Data;
};