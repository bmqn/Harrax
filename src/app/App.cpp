#include "App.hpp"

#include "Config.h"
#include "util/Log.h"
#include "util/Time.hpp"
#include "util/Random.hpp"
#include "graphics/Renderer.hpp"
#include "game/Registry.hpp"
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
		auto registry = Registry::Get();
		auto entity = registry->CreateEntity();
		
		registry->AddComponent(entity, TagComponent {
			static_cast<decltype(TagComponent::Tag)>(Random::Bool())
		});
		registry->AddComponent(entity, TransformComponent {
			{ Random::Float<float>(-10.0f, 10.0f), Random::Float<float>(-10.0f, 10.0f),
			 Random::Float<float>(-30.0f, -10.0f) },
			{ Random::Float(.1f, .5f), Random::Float(.1f, .5f), Random::Float(.1f, .5f) },
			{ Random::Float<float>(), Random::Float<float>(), Random::Float<float>() }
		});
		registry->AddComponent(entity, RenderComponent {
			{ Random::Float<float>(), Random::Float<float>(), Random::Float<float>(), 1.0f }
		});
		registry->AddComponent(entity, PhysicsComponent {
			{ 0.0f, -Random::Float<float>(), 0.0f }
		});
	}

	auto renderSys = Registry::Get()->CreateSystem<RenderSystem>();
	auto coolSys = Registry::Get()->CreateSystem<CoolSystem>();

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
			coolSys->Update(static_cast<float>(k_TimeStep));

			lag -= k_TimeStep;
		}

		static float s_Angle = 0.0f;
		s_Angle += static_cast<float>(delta * 0.5);

		// Render
		Renderer::BeginScene(
			glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 50.0f)
			* glm::translate(glm::mat4(1.0f), glm::vec3{0.0f, 0.0f, -20.0f})
			* glm::rotate(glm::mat4(1.0f), s_Angle, glm::vec3{0.0f, 1.0f, 0.0f})
			* glm::translate(glm::mat4(1.0f), glm::vec3{0.0f, 0.0f, 20.0f})
		);
		renderSys->Render();
		Renderer::EndScene();

		// Swap buffers
		m_Window->SwapBuffers();
	}

	// Terminate
	Renderer::Terminate();

	m_Window->Destroy();
	Window::Terminate();
}