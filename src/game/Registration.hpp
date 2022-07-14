#pragma once

#include "app/Input.hpp"
#include "graphics/Renderer.hpp"
#include "maths/Algebra.hpp"
#include "util/Random.hpp"

#include <functional>

#include <glm/glm.hpp>

//-------------------------------------------------------------------------------------------------
//	Components
//-------------------------------------------------------------------------------------------------

struct TransformComponent
{
	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 Rotation;

	TransformComponent() = default;
	TransformComponent(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
		: Position(position)
		, Scale(scale)
		, Rotation(rotation)
	{}
};

DECL_COMPONENT(TransformComponent)

struct PhysicsComponent
{
	bool Active;
	glm::vec3 Velocity;
	glm::vec3 Acceleration;
	
	PhysicsComponent() = default;
	PhysicsComponent(glm::vec3 velocity, glm::vec3 acceleration = glm::vec3(0.0f))
		: Active(true)
		, Velocity(velocity)
		, Acceleration(acceleration)
	{}
};

DECL_COMPONENT(PhysicsComponent)

struct SpriteComponent
{
	bool Visible;
	bool Billboard;
	glm::vec4 Colour;

	SpriteComponent() = default;
	SpriteComponent(glm::vec4 colour)
		: Visible(true)
		, Billboard(false)
		, Colour(colour)
	{}
};

DECL_COMPONENT(SpriteComponent)

struct MeshComponent
{
	bool Visible = true;
	glm::vec4 Colour;

	MeshComponent() = default;
	MeshComponent(glm::vec4 colour)
		: Visible(true)
		, Colour(colour)
	{}
};

DECL_COMPONENT(MeshComponent)

struct ParticleEmitter
{
	float Lifetime;
	float LifetimeVariation;

	float Speed;
	float SpeedVariation;
	
	float EmissionPeriod;
	float EmissionPeriodCount;
	
	glm::vec3 Direction;
	glm::vec3 DirectionVariation;

	glm::vec4 InitialColour;
	glm::vec4 FinalColour;

	ParticleEmitter() = default;
};

DECL_COMPONENT(ParticleEmitter)

template<typename T>
struct InterpolationComponent
{
	bool Active;
	float Period;
	float PeriodCount;
	T IntialValue;
	T FinalValue;
	T *Value;

	void (*OnDoneCallback)(uint32_t);

	InterpolationComponent() = default;
	InterpolationComponent(float period, T initialValue, T finalValue, T *value)
		: Active(true)
		, Period(period)
		, PeriodCount(0.0f)
		, InitialValue(initialValue)
		, FinalValue(finalValue)
		, Value(value)
		, OnDoneCallback(nullptr)
	{}
};

DECL_COMPONENT_TEMPLATED(InterpolationComponent, float)

using namespace glm;
DECL_COMPONENT_TEMPLATED(InterpolationComponent, vec4)

//-------------------------------------------------------------------------------------------------
//	Systems
//-------------------------------------------------------------------------------------------------

class SpriteRenderSystem : public System
{
public:
	void Render()
	{
		Registry *reg = Registry::Get();

		for (uint32_t entityId : m_Entities)
		{
			const auto &sprite = reg->GetComponent<SpriteComponent>(entityId);
			if (sprite.Visible)
			{
				const auto &transform = reg->GetComponent<TransformComponent>(entityId);
				
				glm::vec3 rotation = sprite.Billboard
					? -Renderer::GetCamera().Rotation
					: transform.Rotation;

				auto vertices = MakeQuadVertices(
					transform.Position, transform.Scale, rotation
				);
				glm::vec4 colour = sprite.Colour;

				Renderer::SubmitQuad(vertices, colour);
			}
		}
	}
};

DECL_SYSTEM(
	SpriteRenderSystem,
	TransformComponent, SpriteComponent
)

class MeshRenderSystem : public System
{
public:
	void Render()
	{
		Registry *reg = Registry::Get();

		for (uint32_t entityId : m_Entities)
		{
			const auto &mesh = reg->GetComponent<MeshComponent>(entityId);
			if (mesh.Visible)
			{
				const auto &transform = reg->GetComponent<TransformComponent>(entityId);
				auto vertices = MakeCubeVertices(
					transform.Position, transform.Scale, transform.Rotation
				);
				glm::vec4 colour = mesh.Colour;

				Renderer::SubmitCube(vertices, colour);
			}
		}
	}
};

DECL_SYSTEM(
	MeshRenderSystem,
	TransformComponent, MeshComponent
)

class PhysicsSystem : public System
{
	static constexpr glm::vec3 k_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

public:
	void Update(float dt)
	{
		Registry *reg = Registry::Get();

		for (uint32_t entityId : m_Entities)
		{
			auto &physics = reg->GetComponent<PhysicsComponent>(entityId);
			if (physics.Active)
			{
				auto &transform = reg->GetComponent<TransformComponent>(entityId);
				physics.Velocity += (physics.Acceleration + k_Gravity) * dt;
				transform.Position += physics.Velocity * dt;
			}
		}
	}
};

DECL_SYSTEM(
	PhysicsSystem,
	TransformComponent, PhysicsComponent
)

class ParticleEmitterSystem : public System
{
	static constexpr size_t k_MaxParticles = 4 * 1024; 

public:
	ParticleEmitterSystem()
	{
		Registry *reg = Registry::Get();

		m_ParticleEntities.resize(k_MaxParticles);
		for (uint32_t &entityId : m_ParticleEntities)
		{
			entityId = reg->CreateEntity();
			reg->AddComponent(entityId, TransformComponent());
			reg->AddComponent(entityId, PhysicsComponent());
			reg->AddComponent(entityId, SpriteComponent());
			reg->AddComponent(entityId, InterpolationComponent<glm::vec4>());

			auto &physics = reg->GetComponent<PhysicsComponent>(entityId);
			physics.Active = false;

			auto &sprite = reg->GetComponent<SpriteComponent>(entityId);
			sprite.Visible = false;

			auto &interp = reg->GetComponent<InterpolationComponent<glm::vec4>>(entityId);
			interp.Active = false;
			interp.OnDoneCallback = &ParticleDiedCallback;
		}

		m_ParticleEntityIndex = 0;
	}

	void Update(float dt)
	{
		Registry *reg = Registry::Get();
		
		for (uint32_t entityId : m_Entities)
		{
			auto &particleEmitter = reg->GetComponent<ParticleEmitter>(entityId);

			if (particleEmitter.EmissionPeriodCount >= particleEmitter.EmissionPeriod)
			{
				particleEmitter.EmissionPeriodCount = 0.0f;

				uint32_t particleEntityId = m_ParticleEntities[m_ParticleEntityIndex];

				auto &transform = reg->GetComponent<TransformComponent>(entityId);
				auto &particleTransform = reg->GetComponent<TransformComponent>(particleEntityId);
				particleTransform.Position = transform.Position;
				particleTransform.Scale = glm::vec3(0.1f);

				glm::vec3 direction = particleEmitter.Direction;
				direction.x += particleEmitter.DirectionVariation.x * Random::Float(-1.0f, 1.0f);
				direction.y += particleEmitter.DirectionVariation.y * Random::Float(-1.0f, 1.0f);
				direction.y += particleEmitter.DirectionVariation.z * Random::Float(-1.0f, 1.0f);
				float speed = particleEmitter.Speed;
				speed += Random::Float(-1.0f, 1.0f) * particleEmitter.SpeedVariation;
				glm::vec3 velocity = direction * speed;
				auto &particlePhysics = reg->GetComponent<PhysicsComponent>(particleEntityId);
				particlePhysics.Active = true;
				particlePhysics.Velocity = velocity;

				glm::vec4 colour = particleEmitter.InitialColour;
				auto &particleSprite = reg->GetComponent<SpriteComponent>(particleEntityId);
				particleSprite.Colour = colour;
				particleSprite.Visible = true;
				particleSprite.Billboard = true;

				float lifetime = particleEmitter.Lifetime;
				lifetime += Random::Float(-1.0f, 1.0f) * particleEmitter.LifetimeVariation;
				auto &particleInterp = reg->GetComponent<InterpolationComponent<glm::vec4>>(particleEntityId);
				particleInterp.Active = true;
				particleInterp.Period = lifetime;
				particleInterp.PeriodCount = 0.0f;
				particleInterp.IntialValue = particleEmitter.InitialColour;
				particleInterp.FinalValue = particleEmitter.FinalColour;
				// TODO: This is BAD but convenient.. this pointer could become invalid if the
				// entities move around. Hopefully this won't happen much when playing.
				particleInterp.Value = &particleSprite.Colour;

				m_ParticleEntityIndex = (m_ParticleEntityIndex + 1) % k_MaxParticles;
			}

			particleEmitter.EmissionPeriodCount += dt;
		}
	}

private:
	static void ParticleDiedCallback(uint32_t entityId)
	{
		Registry *reg = Registry::Get();
		reg->GetComponent<SpriteComponent>(entityId).Visible = false;
		reg->GetComponent<PhysicsComponent>(entityId).Active = false;
		reg->GetComponent<InterpolationComponent<glm::vec4>>(entityId).Active = false;
	}

private:
	std::vector<uint32_t> m_ParticleEntities;
	size_t m_ParticleEntityIndex;
};

DECL_SYSTEM(
	ParticleEmitterSystem,
	TransformComponent, ParticleEmitter
)

template<typename T>
class InterpolatorSystem : public System
{
	static constexpr size_t k_MaxParticles = 1024; 

public:
	void Update(float dt)
	{
		Registry *reg = Registry::Get();
		
		for (uint32_t entityId : m_Entities)
		{
			auto &interp = reg->GetComponent<InterpolationComponent<T>>(entityId);
			if (interp.Active)
			{
				if (interp.PeriodCount >= interp.Period)
				{
					if (interp.Value)
					{
						*interp.Value = interp.FinalValue;
					}

					interp.Active = false;
					interp.OnDoneCallback(entityId);
				}

				if (interp.Value)
				{
					float ratio = interp.PeriodCount / interp.Period;
					*interp.Value = (1.0f - ratio) * interp.IntialValue + ratio * interp.FinalValue;
				}

				interp.PeriodCount += dt;
			}
		}
	}
};

DECL_SYSTEM_TEMPLATED(
	InterpolatorSystem, float,
	InterpolationComponent<float>
)

using namespace glm;
DECL_SYSTEM_TEMPLATED(
	InterpolatorSystem, vec4,
	InterpolationComponent<vec4>
)