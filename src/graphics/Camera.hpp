#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

class Camera
{
public:
	Camera()
		: m_Fov(90.0f), m_Aspect(16.0f / 9.0f), m_Near(0.1f), m_Far(100.0f)
	{
		m_View = glm::mat4(1.0f);
		m_Perspective = glm::perspective(m_Fov, m_Aspect, m_Near, m_Far);
	}
	
	void SetFov(float fov)
	{
		m_Fov = fov;
		m_Perspective = glm::perspective(m_Fov, m_Aspect, m_Near, m_Far);
	}

	void SetAspect(float aspect)
	{
		m_Aspect = aspect;
		m_Perspective = glm::perspective(m_Fov, m_Aspect, m_Near, m_Far);
	}

	void Set(glm::vec3 position, glm::vec3 rotation)
	{
		m_View = glm::translate(glm::mat4(1.0f), position)
			* glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z)
			* glm::translate(glm::mat4(1.0f), -position);
	}

	void LookAt(glm::vec3 position, glm::vec3 centre, glm::vec3 up)
	{
		m_View = glm::lookAt(position, centre, up);
	}

	glm::mat4 GetViewMatrix() const
	{
		return m_View;
	}

	glm::mat4 GetProjMatrix() const
	{
		return m_Perspective;
	}

private:
	float m_Fov, m_Aspect, m_Near, m_Far;
	glm::mat4 m_View, m_Perspective;
};
