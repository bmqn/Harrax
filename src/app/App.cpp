#include "App.hpp"

#include "Config.h"
#include "util/Log.h"
#include "util/Time.hpp"
#include "util/Random.hpp"
#include "graphics/Renderer.hpp"
#include "game/Registry.hpp"
#include "game/Particles.hpp"
#include "maths/Algebra.hpp"

#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>

void App::Run(const Config &config)
{
	// --- Init ---
	Random::Init();

	Window::Init();
	m_Window = std::make_unique<Window>();
	WindowProps props = { 1280, 720, config.Name };
	if (!m_Window->Create(props, std::bind(&App::OnEvent, this, std::placeholders::_1)))
	{
		ASSERT(false, "Failed to create window !");
		return;
	}

	Input::DisableCursor();
	Input::EnableRawMouseInput();
	
	Renderer::Init();

	Camera camera;
	RenderContext renderContext;
	renderContext.camera = &camera;

	ParticleSystem particleSystem;
	
	for (int i = 0; i < 500; ++i)
	{
		auto entity = Registry::Get()->Create();
		
		Registry::Get()->AddComponent<TransformComponent>(entity,
			glm::vec3{ Random::Float(-10.0f, 10.0f), Random::Float(-10.0f, 10.0f), Random::Float(-30.0f, -10.0f) },
			glm::vec3{ 0.5f, 0.5f, 0.5f },
			glm::vec3{ Random::Float<float>(), Random::Float<float>(), Random::Float<float>() }
		);
		
		Registry::Get()->AddComponent<MeshComponent>(entity,
			glm::vec4{ Random::Float<float>(), Random::Float<float>(), Random::Float<float>(), 1.0f }
		);

		if (Random::Float<float>() < 0.2f)
		{
			Registry::Get()->AddComponent<ParticleEmitter>(entity,
				2.5f, 0.2f, 1.5f, 1.0f, 0.1f,
				glm::vec3{ Random::Float(-1.0f, 1.0f), Random::Float(-1.0f, 1.0f), Random::Float(-1.0f, 1.0f) },
				glm::vec3{ Random::Float(0.3f, 0.5f), Random::Float(0.3f, 0.5f), Random::Float(0.3f, 0.5f) },
				glm::vec4{ Random::Float(0.0f, 0.2f), Random::Float<float>(), Random::Float(0.0f, 0.2f), 1.0f },
				glm::vec4{ Random::Float(0.8f, 1.0f), Random::Float<float>(), Random::Float(0.8f, 1.0f), 1.0f }
			);
		}
	}

	// --- Run ---
	auto before = Time::Seconds();
	auto lag = 0.0;

	m_IsRunning = true;
	while (m_IsRunning)
	{
		auto now = Time::Seconds();
		auto delta = now - before;
		before = now;
		lag += delta;

		if (m_Window->ShouldClose())
		{
			break;
		}

		// Process input / window events
		m_Window->PollEvents();

		static float pitch = 0.0f, yaw = -90.0f;
		static glm::vec3 position = glm::vec3{};

		while (lag >= k_TimeStep)
		{
			float dt = static_cast<float>(k_TimeStep);

			particleSystem.Update(dt);

			Registry::Get()->View<TransformComponent, PhysicsComponent>([&dt](EntId id, auto &transform, auto &physics) {
				static constexpr glm::vec3 k_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);
				if (physics.Active)
				{
					physics.Velocity += (physics.Acceleration + k_Gravity) * dt;
					transform.Position += physics.Velocity * dt;
				}
			});

			static auto lastMouse = Input::GetMousePosition();
			auto nowMouse = Input::GetMousePosition();
			auto deltaMouse = (lastMouse - nowMouse) * dt * 10.0f;
			lastMouse = nowMouse;
			pitch += deltaMouse.y;
			yaw -= deltaMouse.x;

			glm::vec3 look;
			look.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
			look.y = sin(glm::radians(pitch));
			look.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

			glm::vec3 forward = glm::normalize(look);
			glm::vec3 right = glm::normalize(glm::cross(glm::vec3{0.0f, 1.0f, 0.0f}, forward));
			glm::vec3 up = glm::cross(forward, right);

			if (Input::GetKeyDown(GLFW_KEY_W))
			{
				position += forward;
			}
			else if (Input::GetKeyDown(GLFW_KEY_S))
			{
				position -= forward;
			}
			if (Input::GetKeyDown(GLFW_KEY_A))
			{
				position += right;
			}
			else if (Input::GetKeyDown(GLFW_KEY_D))
			{
				position -= right;
			}

			camera.LookAt(position, position + forward, up);

			lag -= k_TimeStep;
		}

		// Render
		Renderer::BeginScene(renderContext);

		Registry::Get()->View<TransformComponent, MeshComponent>([&](EntId id, const auto &transform, const auto &mesh) {
			if (mesh.Visible)
			{
				auto vertices = MakeCubeVertices(
					transform.Position, transform.Scale, transform.Rotation
				);
				Renderer::SubmitCube(vertices, mesh.Colour);
			}
		});

		Registry::Get()->View<TransformComponent, SpriteComponent>([&](EntId id, const auto &transform, const auto &sprite) {
			if (sprite.Visible)
			{
				if (sprite.Billboard)
				{
					glm::vec3 forward = glm::normalize(transform.Position - position);
					glm::vec3 right = glm::normalize(glm::cross(glm::vec3{0.0f, 1.0f, 0.0f}, forward));
					glm::vec3 up = glm::cross(forward, right);

					glm::mat4 billboard = glm::mat4(
						glm::vec4(right, 0), glm::vec4(up, 0),
						glm::vec4(forward, 0), glm::vec4(transform.Position, 1)
					) * glm::scale(glm::mat4(1.0f), glm::vec3(transform.Scale));

					auto vertices = MakeQuadVertices(billboard);
					Renderer::SubmitQuad(vertices, sprite.Colour);
				}
				else
				{
					auto vertices = MakeQuadVertices(
						transform.Position, transform.Scale, transform.Rotation
					);
					Renderer::SubmitQuad(vertices, sprite.Colour);
				}
			}
		});

		Renderer::EndScene();

		// Swap buffers
		m_Window->SwapBuffers();
	}

	// --- Terminate ---
	Renderer::Terminate();

	m_Window->Destroy();
	Window::Terminate();
}

void App::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowResizeEvent>([](WindowResizeEvent &e) {
		Renderer::SetViewportSize(e.Width, e.Height);
		return false;
	});
	dispatcher.dispatch<KeyPressedEvent>([&](KeyPressedEvent &e) {
		if (e.Key == GLFW_KEY_ESCAPE)
		{
			m_IsRunning = false;
		}
		return false;
	});
}