#pragma once

#include "util/Random.hpp"
#include "game/Entity.hpp"
#include "game/System.hpp"
#include "maths/Algebra.hpp"
#include "graphics/Renderer.hpp"

#include <glm/glm.hpp>

//-------------------------------------------------------------------------------------------------
//	Components
//-------------------------------------------------------------------------------------------------

struct TransformComponent
{
	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 Rotation;
};

struct PhysicsComponent
{
	glm::vec3 Velocity;
};

struct RenderComponent
{
	glm::vec4 Colour;
};

DECL_COMPONENT(TransformComponent)
DECL_COMPONENT(PhysicsComponent)
DECL_COMPONENT(RenderComponent)

//-------------------------------------------------------------------------------------------------
//	Systems
//-------------------------------------------------------------------------------------------------

class RenderSystem : public System
{
public:
	void Render()
	{
		for (uint32_t entityId : m_Entities)
		{
			const auto &transform = EntityManager::Get()->GetComponent<TransformComponent>(entityId);
			const auto &render = EntityManager::Get()->GetComponent<RenderComponent>(entityId);

			auto vertices = MakeCubeVertices(
				transform.Position, transform.Scale, transform.Rotation
			);
			glm::vec4 colour = render.Colour;

			Renderer::SubmitCube(vertices, colour);
		}
	}
};

DECL_SYSTEM(
	RenderSystem,
	TransformComponent, RenderComponent
)

class PhysicsSystem : public System
{
public:
	void Update(float dt)
	{
		for (uint32_t entityId : m_Entities)
		{
			auto &transform = EntityManager::Get()->GetComponent<TransformComponent>(entityId);
			auto &physics = EntityManager::Get()->GetComponent<PhysicsComponent>(entityId);

			transform.Position += physics.Velocity * dt;

			transform.Rotation.x += 10.0f * dt * Random::Float<float>();
			transform.Rotation.y += 5.0f * dt * Random::Float<float>();
			transform.Rotation.z += 3.0f * dt * Random::Float<float>();
		}
	}
};

DECL_SYSTEM(
	PhysicsSystem,
	TransformComponent
)