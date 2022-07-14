#pragma once

#include <array>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>

std::array<glm::vec3, 4> MakeQuadVertices(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
	glm::mat4 rot =
		glm::translate(glm::mat4(1.0f), position)
		* glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z)
		* glm::translate(glm::mat4(1.0f), -position);

	glm::vec3 p1;
	p1.x = position.x - scale.x;
	p1.y = position.y - scale.y;
	p1.z = position.z + scale.z;
	p1 = rot * glm::vec4(p1, 1.0f);

	glm::vec3 p2;
	p2.x = position.x - scale.x;
	p2.y = position.y + scale.y;
	p2.z = position.z + scale.z;
	p2 = rot * glm::vec4(p2, 1.0f);

	glm::vec3 p3;
	p3.x = position.x + scale.x;
	p3.y = position.y - scale.y;
	p3.z = position.z + scale.z;
	p3 = rot * glm::vec4(p3, 1.0f);
	
	glm::vec3 p4;
	p4.x = position.x + scale.x;
	p4.y = position.y + scale.y;
	p4.z = position.z + scale.z;
	p4 = rot * glm::vec4(p4, 1.0f);

	return { p1, p2, p3, p4 };
}

std::array<glm::vec3, 8> MakeCubeVertices(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
	glm::mat4 rot =
		glm::translate(glm::mat4(1.0f), position)
		* glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z)
		* glm::translate(glm::mat4(1.0f), -position);

	glm::vec3 p1;
	p1.x = position.x - scale.x;
	p1.y = position.y - scale.y;
	p1.z = position.z + scale.z;
	p1 = rot * glm::vec4(p1, 1.0f);

	glm::vec3 p2;
	p2.x = position.x - scale.x;
	p2.y = position.y + scale.y;
	p2.z = position.z + scale.z;
	p2 = rot * glm::vec4(p2, 1.0f);

	glm::vec3 p3;
	p3.x = position.x + scale.x;
	p3.y = position.y - scale.y;
	p3.z = position.z + scale.z;
	p3 = rot * glm::vec4(p3, 1.0f);
	
	glm::vec3 p4;
	p4.x = position.x + scale.x;
	p4.y = position.y + scale.y;
	p4.z = position.z + scale.z;
	p4 = rot * glm::vec4(p4, 1.0f);
	
	glm::vec3 p5;
	p5.x = position.x - scale.x;
	p5.y = position.y - scale.y;
	p5.z = position.z - scale.z;
	p5 = rot * glm::vec4(p5, 1.0f);

	glm::vec3 p6;
	p6.x = position.x - scale.x;
	p6.y = position.y + scale.y;
	p6.z = position.z - scale.z;
	p6 = rot * glm::vec4(p6, 1.0f);

	glm::vec3 p7;
	p7.x = position.x + scale.x;
	p7.y = position.y - scale.y;
	p7.z = position.z - scale.z;
	p7 = rot * glm::vec4(p7, 1.0f);
	
	glm::vec3 p8;
	p8.x = position.x + scale.x;
	p8.y = position.y + scale.y;
	p8.z = position.z - scale.z;
	p8 = rot * glm::vec4(p8, 1.0f);

	return { p1, p2, p3, p4, p5, p6, p7, p8 };
}