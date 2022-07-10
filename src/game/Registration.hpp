#pragma once

#include "app/Input.hpp"
#include "graphics/Renderer.hpp"
#include "maths/Algebra.hpp"
#include "util/Random.hpp"

#include <glm/glm.hpp>

//-------------------------------------------------------------------------------------------------
//	Components
//-------------------------------------------------------------------------------------------------

struct TagComponent
{
	enum
	{
		OFF = 0,
		ON = 1
	} Tag;
};

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

DECL_COMPONENT(TagComponent)
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
			const auto &transform = Registry::Get()->GetComponent<TransformComponent>(entityId);
			const auto &render = Registry::Get()->GetComponent<RenderComponent>(entityId);

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

class CoolSystem : public System
{
public:
	void Update(float dt)
	{
		static glm::vec2 s_LastPos = Input::GetMousePosition();
		glm::vec2 thisPos = Input::GetMousePosition();

		for (uint32_t entityId : m_Entities)
		{
			auto &transform = Registry::Get()->GetComponent<TransformComponent>(entityId);
			auto &tag = Registry::Get()->GetComponent<TagComponent>(entityId);

			if (tag.Tag == TagComponent::ON)
			{
				transform.Rotation.x += (thisPos.x - s_LastPos.x) * 0.05f * Random::Float<float>();
				transform.Rotation.y += (thisPos.y - s_LastPos.y) * 0.05f * Random::Float<float>();
			}
		}

		s_LastPos = thisPos;
	}
};

DECL_SYSTEM(
	CoolSystem,
	TransformComponent, TagComponent
)