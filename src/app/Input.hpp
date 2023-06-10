#pragma once

#include <glm/glm.hpp>

class Input
{
public:
	static bool GetKeyDown(int key);

	static void DisableCursor();
	static void EnableCursor();
	static void DisableRawMouseInput();
	static void EnableRawMouseInput();
	static glm::vec2 GetMousePosition();
};