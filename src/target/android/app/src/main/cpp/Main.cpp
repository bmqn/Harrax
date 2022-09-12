#include "app/App.hpp"
#include "app/Window.hpp"
#include "util/Log.h"

#include <thread>

#include <jni.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>

static ANativeWindow *s_NativeWindow = nullptr;
static std::thread s_RunThread;

extern "C"
{
	JNIEXPORT void JNICALL Java_com_harrax_HarraxLib_start(JNIEnv *env, jobject obj, jobject surface);
	JNIEXPORT void JNICALL Java_com_harrax_HarraxLib_end(JNIEnv *env, jobject obj);
	JNIEXPORT void JNICALL Java_com_harrax_HarraxLib_surfaceChanged(JNIEnv *env, jobject obj, jobject surface);
};

JNIEXPORT void JNICALL Java_com_harrax_HarraxLib_start(JNIEnv *env, jobject obj, jobject surface)
{
	s_NativeWindow = ANativeWindow_fromSurface(env, surface);
	Window::SetNativeWindow(s_NativeWindow);

	s_RunThread = std::thread([](){
		App::Get()->Run();
	});
}

JNIEXPORT void JNICALL Java_com_harrax_HarraxLib_end(JNIEnv *env, jobject obj)
{
	Window::SetShouldClose();

	s_RunThread.join();
}

JNIEXPORT void JNICALL Java_com_harrax_HarraxLib_surfaceChanged(JNIEnv *env, jobject obj, jobject surface)
{

}