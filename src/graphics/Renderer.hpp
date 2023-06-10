#pragma once

#include "Camera.hpp"

#include <array>

struct RenderContext
{
	const Camera *camera;
};

class Renderer
{
private:
	static void InitRenderer();
	static void CleanupRenderer();
	static void FlushVertices();
	static void FlushScene();

	static void MapBuffer();
	static void UnmapBuffer();

public:
	static void Init();
	static void Terminate();

	static void SetViewportSize(int width, int height);

	static void BeginScene(const RenderContext &context);
	static void EndScene();

	static void SubmitTriangle(const std::array<glm::vec3, 3> &vertices, glm::vec4 colour);
	static void SubmitQuad(const std::array<glm::vec3, 4> &vertices, glm::vec4 colour);
	static void SubmitCube(const std::array<glm::vec3, 8> &vertices, glm::vec4 colour);
};
