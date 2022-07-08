#pragma once

#include "Entity.hpp"

#include <glm/glm.hpp>

struct TransformComponent
{
	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 Rotation;
};

struct RenderComponent
{
	glm::vec4 Colour;
};

DECL_COMPONENT(TransformComponent)
DECL_COMPONENT(RenderComponent)