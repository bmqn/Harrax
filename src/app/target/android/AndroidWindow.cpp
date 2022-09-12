#include "../../Window.hpp"

#include "util/Log.h"

#include <android/native_window.h>

#include <EGL/egl.h>

static ANativeWindow *s_NativeWindow = nullptr;
static bool s_ShouldClose = false;

void Window::SetNativeWindow(ANativeWindow *windowHandle)
{
	s_NativeWindow = windowHandle;
}

void Window::SetShouldClose()
{
	s_ShouldClose = true;
}

struct EGLData
{
	EGLDisplay Display;
	EGLSurface Surface;
	EGLContext Context;
	EGLConfig Config;

	EGLData()
		: Display(EGL_NO_DISPLAY)
		, Surface(EGL_NO_SURFACE)
		, Context(EGL_NO_CONTEXT)
		, Config(0)
	{
	}
};

static EGLData s_EGLData;

void Window::Init()
{
	if (s_EGLData.Display != EGL_NO_DISPLAY)
	{
		return;
	}

	s_EGLData.Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (EGL_FALSE == eglInitialize(s_EGLData.Display, 0, 0))
	{
		ASSERT(false, "Failed to init display, error %d", eglGetError());
		return;
	}

	if (s_EGLData.Surface != EGL_NO_SURFACE)
	{
		return;
	}

	EGLint numConfigs;
	const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, 16,
			EGL_NONE
	};
	if (EGL_FALSE == eglChooseConfig(
			s_EGLData.Display, attribs, &s_EGLData.Config, 1, &numConfigs
	))
	{
		ASSERT(false, "Failed to choose an EGL config, EGL error %d", eglGetError());
		return;
	}

	s_EGLData.Surface = eglCreateWindowSurface(
			s_EGLData.Display, s_EGLData.Config, s_NativeWindow, NULL
	);
	if (s_EGLData.Surface == EGL_NO_SURFACE)
	{
		ASSERT(false, "Failed to create EGL surface, EGL error %d", eglGetError());
		return;
	}

	if (s_EGLData.Context != EGL_NO_CONTEXT)
	{
		return;
	}

	EGLint attribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	s_EGLData.Context = eglCreateContext(
			s_EGLData.Display, s_EGLData.Config, NULL, attribList
	);
	if (s_EGLData.Context == EGL_NO_CONTEXT) {
		ASSERT(false, "Failed to create EGL context, EGL error %d", eglGetError());
		return;
	}

	if(eglMakeCurrent(s_EGLData.Display, s_EGLData.Surface,
					  s_EGLData.Surface, s_EGLData.Context) == EGL_FALSE
	)
	{
		ASSERT(false, "Failed to make EGL context current, EGL error %d", eglGetError());
		return;
	}
}

void Window::Terminate()
{
}

bool Window::Create(const WindowProps& props)
{
	if (m_Data)
	{
		LOG("Window has already been created !");
		return false;
	}

	EGLint width;
	EGLint height;
	eglQuerySurface(s_EGLData.Display, s_EGLData.Surface, EGL_WIDTH, &width);
	eglQuerySurface(s_EGLData.Display, s_EGLData.Surface, EGL_HEIGHT, &height);

	m_Data = std::make_unique<WindowData>();
	m_Data->Width = width;
	m_Data->Height = height;
	m_Data->Title = props.Title;

	return true;
}

void Window::Destroy()
{
}

bool Window::ShouldClose()
{
	return s_ShouldClose;
}

void Window::PollEvents()
{
}

void Window::SwapBuffers()
{
	if (EGL_FALSE == eglSwapBuffers(s_EGLData.Display, s_EGLData.Surface))
	{
		ASSERT(false, "Failed to swap EGL buffers, EGL error %d", eglGetError());
	}
}