#pragma once

#include "app/App.hpp"

#include <array>

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

struct Camera
{
	glm::mat4 GetViewMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), Position)
				* glm::eulerAngleYXZ(Rotation.y, Rotation.x, Rotation.z)
				* glm::translate(glm::mat4(1.0f), -Position);
	}

	glm::mat4 GetProjMatrix() const
	{
		float width = static_cast<float>(App::Get()->GetWindow().GetWidth());
		float height = static_cast<float>(App::Get()->GetWindow().GetHeight());
		return glm::perspective(45.0f, width / height, 0.1f, 100.0f);
	}

	glm::vec3 Position;
	glm::vec3 Rotation;
};

class Renderer
{
public:
	static void Init();
	static void Terminate();

	static void BeginScene(const Camera &camera);
	static void EndScene();

	static const Camera &GetCamera();

	static void SubmitTriangle(const std::array<glm::vec3, 3> &vertices, glm::vec4 colour);
	static void SubmitQuad(const std::array<glm::vec3, 4> &vertices, glm::vec4 colour);
	static void SubmitCube(const std::array<glm::vec3, 8> &vertices, glm::vec4 colour);
};
