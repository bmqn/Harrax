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

	for (int i = 0; i < 100; ++i)
	{
		auto registry = Registry::Get();
		auto entity = registry->CreateEntity();
		
		registry->AddComponent(entity, TransformComponent(
			{ Random::Float(-10.0f, 10.0f), Random::Float(-10.0f, 10.0f), Random::Float(-30.0f, -10.0f) },
			{ 0.5f, 0.5f, 0.5f },
			{ Random::Float<float>(), Random::Float<float>(), Random::Float<float>() }
		));
		
		registry->AddComponent(entity, MeshComponent(
			{ Random::Float<float>(), Random::Float<float>(), Random::Float<float>(), 1.0f }
		));

		if (Random::Float<float>() < 0.2f)
		{
			registry->AddComponent(entity, ParticleEmitter {
				1.5f, 0.2f,
				3.0f, 1.0f,
				0.1f, 0.05f,
				{ Random::Float(-1.0f, 1.0f), Random::Float(-1.0f, 1.0f), Random::Float(-1.0f, 1.0f) },
				{ Random::Float(0.3f, 0.5f), Random::Float(0.3f, 0.5f), Random::Float(0.3f, 0.5f) },
				{ Random::Float(0.0f, 0.2f), Random::Float<float>(), Random::Float(0.0f, 0.2f), 1.0f },
				{ Random::Float(0.8f, 1.0f), Random::Float<float>(), Random::Float(0.8f, 1.0f), 1.0f }
			});
		}
	}

	auto spriteRenderSys = Registry::Get()->CreateSystem<SpriteRenderSystem>();
	auto meshRenderSys = Registry::Get()->CreateSystem<MeshRenderSystem>();
	auto physicsSys = Registry::Get()->CreateSystem<PhysicsSystem>();
	auto particleEmitterSys = Registry::Get()->CreateSystem<ParticleEmitterSystem>();
	auto interpolatorSys = Registry::Get()->CreateSystem<InterpolatorSystem<glm::vec4>>();

	Camera camera;
	camera.Position = { 0.0f, 0.0f, -20.0f };
	camera.Rotation = { 0.0f, 0.0f, 0.0f };

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
			Registry::Get()->Update();

			// Update logic
			physicsSys->Update(static_cast<float>(k_TimeStep));
			particleEmitterSys->Update(static_cast<float>(k_TimeStep));
			interpolatorSys->Update(static_cast<float>(k_TimeStep));

			lag -= k_TimeStep;
		}

		static float s_Angle = 0.0f;
		s_Angle += static_cast<float>(delta);
		camera.Rotation.y = s_Angle * 0.4f;
		// camera.Rotation.z = s_Angle * 0.3f;

		// Render
		Renderer::BeginScene(camera);
		meshRenderSys->Render();
		spriteRenderSys->Render();
		Renderer::EndScene();

		// Swap buffers
		m_Window->SwapBuffers();
	}

	// Terminate
	Renderer::Terminate();

	m_Window->Destroy();
	Window::Terminate();
}