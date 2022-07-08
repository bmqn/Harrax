#include "App.hpp"

#include "Config.h"
#include "util/Log.h"
#include "util/Time.hpp"
#include "util/Random.hpp"
#include "graphics/Renderer.hpp"
#include "game/Entity.hpp"
#include "maths/Algebra.hpp"

#include <glm/ext.hpp>

void App::Run()
{
	// Init
	Random::Init();

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

	for (int i = 0; i < 500; ++i)
	{
		auto ent = EntityManager::Get()->Create();
		TransformComponent t = {
			{Random::Float<float>(-5.0f, 5.0f), Random::Float<float>(-5.0f, 5.0f), Random::Float<float>(-15.0f, -5.0f)},
			{0.2f, 0.2f, 0.2f},
			{Random::Float<float>(), Random::Float<float>(), Random::Float<float>()}
		};
		RenderComponent r = {
			{Random::Float<float>(), Random::Float<float>(), Random::Float<float>(), 1.0f}
		};
		EntityManager::Get()->AddComponent(ent, t);
		EntityManager::Get()->AddComponent(ent, r);
	}

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

		// Render
		Renderer::BeginScene(
			glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 50.0f)
			* glm::translate(glm::mat4(1.0f), glm::vec3{0.0f, 0.0f, -10.0f})
			* glm::rotate(glm::mat4(1.0f), s_Angle, glm::vec3{0.0f, 1.0f, 0.0f})
			* glm::translate(glm::mat4(1.0f), glm::vec3{0.0f, 0.0f, 10.0f})
		);
		EntityManager::Get()->View<TransformComponent, RenderComponent>([&](uint32_t entity) {
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