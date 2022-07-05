#include "App.hpp"

#include "Config.h"
#include "util/Log.h"
#include "util/Time.hpp"
#include "graphics/Renderer.hpp"
#include "game/Entity.hpp"
#include "maths/Algebra.hpp"

#include <glm/ext.hpp>

void App::Run()
{
	// Init
	Window::Init();
	WindowProps props = {
		1280, 720, "App"
	};
	m_Window = std::make_unique<Window>();
	if (!m_Window->Create(props))
	{
		ASSERT(false, "Failed to create window !");
		return;
	}
	
	Renderer::Init();

	auto ent1 = EntityManager::Get()->Create();
	TransformComponent t1 = {
		{0.0f, 0.0f, -1.0f},
		{0.2f, 0.2f, 0.2f}
	};
	RenderComponent r1 = {
		{0.2f, 0.8f, 0.6f, 1.0f}
	};
	EntityManager::Get()->AddComponent(ent1, t1);
	EntityManager::Get()->AddComponent(ent1, r1);

	// Run
	auto before = Time::Millis();
	auto lag = 0.0;

	while (true)
	{
		auto now = Time::Millis();
		auto delta = now - before;
		before = now;
		lag += delta;

		if (m_Window->ShouldClose())
		{
			break;
		}

		// Process input / window events
		m_Window->PollEvents();

		while (lag >= k_TimeStep)
		{
			// Update logic
			
			lag -= k_TimeStep;
		}

		static float s_Angle = 0.0f;
		s_Angle += static_cast<float>(delta);

		auto &transform = EntityManager::Get()->GetComponent<TransformComponent>(0);
		transform.Rotation.x = s_Angle;
		transform.Rotation.y = -s_Angle * 0.2f;
		transform.Rotation.z = s_Angle * 0.8f;

		// Render
		Renderer::Clear();
		Renderer::BeginScene(glm::perspective(70.0f, 16.0f / 9.0f, 0.1f, 50.0f));
		EntityManager::Get()->View<TransformComponent, RenderComponent>([&](Ent entity) {
			const auto &transform = EntityManager::Get()->GetComponent<TransformComponent>(entity);
			const auto &render = EntityManager::Get()->GetComponent<RenderComponent>(entity);

			auto vertices = MakeCubeVertices(
				transform.Position, transform.Scale, transform.Rotation
			);
			glm::vec4 colour = render.Colour;

			Renderer::SubmitCube(vertices, colour);
		});
		Renderer::EndScene();

		// Swap buffers
		m_Window->SwapBuffers();
	}

	// Terminate
	Renderer::Terminate();

	m_Window->Destroy();
	Window::Terminate();
}