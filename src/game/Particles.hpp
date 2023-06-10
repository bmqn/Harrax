#pragma once

#include "app/Input.hpp"
#include "graphics/Renderer.hpp"
#include "maths/Algebra.hpp"
#include "util/Random.hpp"

#include <functional>
#include <unordered_map>

class ParticleSystem
{
	static constexpr size_t k_MaxParticles = 4 * 1024; 

public:
	ParticleSystem()
	{
		Registry *reg = Registry::Get();

		m_NextParticleIndex = 0;
		m_Particles.resize(k_MaxParticles);
		for (auto &particle : m_Particles)
		{
			particle.Id = reg->Create();
			particle.Lifetime = 0.0f;
			particle.Alivetime = 0.0f;
			particle.Alive = false;

			reg->AddComponent<TransformComponent>(particle.Id);
			reg->AddComponent<PhysicsComponent>(particle.Id);
			reg->AddComponent<SpriteComponent>(particle.Id);
			
			auto &physics = reg->GetComponent<PhysicsComponent>(particle.Id);
			physics.Active = false;
			auto &sprite = reg->GetComponent<SpriteComponent>(particle.Id);
			sprite.Visible = false;
			sprite.Billboard = true;
		}
	}

	void Update(float dt)
	{
		Registry *reg = Registry::Get();

		reg->View<TransformComponent, ParticleEmitter>([&](EntId id, const auto &transform, const auto &emitter) {
			auto &system = m_Systems[id];
			if (system.EmissionPeriodTime >= emitter.EmissionPeriod)
			{
				auto &particle = m_Particles[m_NextParticleIndex];
				if (!particle.Alive)
				{
					particle.Lifetime = Random::Float(emitter.Lifetime - emitter.LifetimeVariation, emitter.Lifetime + emitter.LifetimeVariation);
					particle.Alivetime = 0.0f;
					particle.Alive = true;

					auto &particleTransform = reg->GetComponent<TransformComponent>(particle.Id);
					particleTransform.Position = transform.Position;
					particleTransform.Scale = glm::vec3(0.1f);

					glm::vec3 dir = emitter.Direction;
					dir.x += emitter.DirectionVariation.x * Random::Float(-1.0f, 1.0f);
					dir.y += emitter.DirectionVariation.y * Random::Float(-1.0f, 1.0f);
					dir.y += emitter.DirectionVariation.z * Random::Float(-1.0f, 1.0f);
					float speed = Random::Float(emitter.Speed - emitter.SpeedVariation, emitter.Speed + emitter.SpeedVariation);
					auto &particlePhysics = reg->GetComponent<PhysicsComponent>(particle.Id);
					particlePhysics.Velocity = dir * speed;
					particlePhysics.Active = true;

					glm::vec4 colour = emitter.InitialColour;
					auto &particleSprite = reg->GetComponent<SpriteComponent>(particle.Id);
					particleSprite.Colour = colour;
					particleSprite.Visible = true;

					m_NextParticleIndex = (m_NextParticleIndex + 1) % k_MaxParticles;
				}
				system.EmissionPeriodTime = 0.0f;
			}
		});

		for (auto it = m_Systems.begin(); it != m_Systems.end(); ++it)
		{
			it->second.EmissionPeriodTime += dt;
		}

		for (auto &particle : m_Particles)
		{
			if (particle.Alive)
			{
				if (particle.Alivetime >= particle.Lifetime)
				{
					particle.Alive = false;

					auto &physics = reg->GetComponent<PhysicsComponent>(particle.Id);
					physics.Active = false;
					auto &sprite = reg->GetComponent<SpriteComponent>(particle.Id);
					sprite.Visible = false;
				}

				particle.Alivetime += dt;
			}
		}
	}

private:
	struct Particle
	{
		EntId Id;
		float Alivetime = 0.0f, Lifetime = 0.0f;
		bool Alive = false;
	};

	struct System
	{
		float EmissionPeriodTime = std::numeric_limits<float>::infinity();
	};

private:
	size_t m_NextParticleIndex;
	std::vector<Particle> m_Particles;
	std::unordered_map<EntId, System> m_Systems;
};
