#include "camera.h"

// constructor
Camera::Camera(glm::vec3 position)
	: cameraPos(position),
	worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	yaw(0.0f),
	pitch(0.0f),
	speed(2.5f),
	sensitivity(1.0f),
	zoom(45.0f),
	cameraFront(glm::vec3(1.0f, 0.0f, 0.0f))
{
	updateCameraVectors();
}

// actualizar direccion
void Camera::updateCameraDirection(double dx, double dy)
{
	yaw += dx;
	pitch += dy;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	updateCameraVectors();
}

// actualizar posicion
void Camera::updateCameraPos(CameraDirection dir, double dt)
{
	float velocity = (float)dt * speed;

	switch (dir)
	{
	case CameraDirection::FORWARD:
		cameraPos += cameraFront * velocity;
		break;
	case CameraDirection::BACKWARD:
		cameraPos -= cameraFront * velocity;
		break;
	case CameraDirection::RIGHT:
		cameraPos += cameraRight * velocity;
		break;
	case CameraDirection::LEFT:
		cameraPos -= cameraRight * velocity;
		break;
	case CameraDirection::UP:
		cameraPos += worldUp * velocity;
		break;
	case CameraDirection::DOWN:
		cameraPos -= worldUp * velocity;
		break;
	}
}

// actualizar zoom
void Camera::updateCameraZoom(double dy)
{
	if (zoom >= 1.0f && zoom <= 45.0f)
	{
		zoom -= dy;
	}
	else if (zoom < 1.0f)
	{
		zoom = 1.0f;
	}
	else
	{
		zoom = 45.0f;
	}
}

// zoom
float Camera::getZoom() {
	return zoom;
}

// matriz de vista
glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

// actualizar vectores
void Camera::updateCameraVectors()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}