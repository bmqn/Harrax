#pragma once

#include <glm/glm.hpp>

//-------------------------------------------------------------------------------------------------
//	Components
//-------------------------------------------------------------------------------------------------

struct TransformComponent
{
	glm::vec3 Position = glm::vec3{0, 0, 0};
	glm::vec3 Scale = glm::vec3{1, 1, 1};
	glm::vec3 Rotation = glm::vec3{0, 0, 0};
};

DECL_COMPONENT(TransformComponent)

struct PhysicsComponent
{
	glm::vec3 Velocity = glm::vec3{0, 0, 0};
	glm::vec3 Acceleration = glm::vec3{0, 0, 0};
	bool Active = true;
};

DECL_COMPONENT(PhysicsComponent)

struct SpriteComponent
{
	glm::vec4 Colour = glm::vec4{1, 1, 1, 1};
	bool Visible = true;
	bool Billboard = false;
};

DECL_COMPONENT(SpriteComponent)

struct MeshComponent
{
	glm::vec4 Colour = glm::vec4{1, 1, 1, 1};
	bool Visible = true;
};

DECL_COMPONENT(MeshComponent)

struct ParticleEmitter
{
	float Lifetime;
	float LifetimeVariation;

	float Speed;
	float SpeedVariation;
	
	float EmissionPeriod;
	
	glm::vec3 Direction;
	glm::vec3 DirectionVariation;

	glm::vec4 InitialColour;
	glm::vec4 FinalColour;
};

DECL_COMPONENT(ParticleEmitter)