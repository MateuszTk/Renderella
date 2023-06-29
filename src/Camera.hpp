#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "WindowManager.hpp"

class Camera {
public:
	Camera(bool controllable = false, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), float fov = 65.0f, float farPlane = 400.0f, float nearPlane = 0.1f)
		: cameraMatrix(1.0f), fov(fov), farPlane(farPlane), nearPlane(nearPlane), controllable(controllable) {
		this->position = position;
		this->rotation = rotation;
	}

	const glm::vec3& getPosition() const {
		return position;
	}

	const glm::vec3& getRotation() const {
		return rotation;
	}

	void setPosition(const glm::vec3& position) {
		this->position = position;
	}

	void setRotation(const glm::vec3& rotation) {
		this->rotation = rotation;
	}

	void update(const WindowManager& windowManager) {
		if (this->controllable) {
			processInput(windowManager);
			updateMatrix(windowManager);
		}		
	}

	void use() {
		activeCamera = this;
	}

	const glm::mat4& getCameraMatrix() {
		return this->cameraMatrix;
	}

	static Camera* getActiveCamera() {
		return activeCamera;
	}

	void setFov(float fov) {
		this->fov = fov;
	}

	float getFov() const {
		return this->fov;
	}

	void setClipplingPlanes(float nearPlane, float farPlane) {
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
	}

	float getNearPlane() const {
		return this->nearPlane;
	}

	float getFarPlane() const {
		return this->farPlane;
	}

	void setSpeed(float speed) {
		this->speed = speed;
	}

	float getSpeed() const {
		return this->speed;
	}

	void setSensitivity(float sensitivity) {
		this->sensitivity = sensitivity;
	}

	float getSensitivity() const {
		return this->sensitivity;
	}

	bool isControllable() const {
		return this->controllable;
	}

	void setControllable(bool controllable) {
		this->controllable = controllable;
	}

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::mat4 cameraMatrix;
	static Camera* activeCamera;

	float fov;
	float nearPlane;
	float farPlane;

	bool controllable = true;
	float speed = 4.0f;
	float sensitivity = 0.1f;

	//variables for mouse and keyboard input
	glm::vec2 lastCursorPos = glm::vec2(0.0f);
	glm::vec2 angle = glm::vec2(0.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
	const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	
	void processInput(const WindowManager& windowManager) {
		glm::vec2 cursor = getCursorsPosition(windowManager);
		glm::vec2 delta = {
			-1.0f * (this->lastCursorPos.x - cursor.x) * sensitivity,
			-1.0f * (this->lastCursorPos.y - cursor.y) * sensitivity
		};
		this->lastCursorPos = cursor;

		cursor.x = this->angle.x + delta.x;
		cursor.y = this->angle.y + delta.y;

		if (cursor.y > +89.0) cursor.y = +89.0;
		if (cursor.y < -89.0) cursor.y = -89.0;

		this->angle.x = cursor.x;
		this->angle.y = cursor.y;

		cursor.x = glm::radians(cursor.x);
		cursor.y = glm::radians(cursor.y);
		this->rotation.x = cursor.x + glm::radians(-90.0f);
		this->rotation.y = cursor.y;

		// vector representing where the camera is currently pointing
		this->direction = {
			 cos(cursor.x) * cos(cursor.y),
			-sin(cursor.y),
			 sin(cursor.x) * cos(cursor.y)
		};

		this->direction = glm::normalize(this->direction);


		float cameraSpeed = windowManager.getDeltaTime() * this->speed;
		auto window = windowManager.getWindow();
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			cameraSpeed *= 4;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			position += direction * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			position -= direction * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			position -= glm::normalize(glm::cross(direction, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			position += glm::normalize(glm::cross(direction, cameraUp)) * cameraSpeed;

		position.y -= (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) ? cameraSpeed : 0;
		position.y += (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) ? cameraSpeed : 0;
	}

	void updateMatrix(const WindowManager& windowManager) {
		glm::mat4 view = glm::lookAt(position, position + direction, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(this->fov), windowManager.getAspectRatio(), this->nearPlane, this->farPlane);
		this->cameraMatrix = projection * view;
	}

	glm::vec2 getCursorsPosition(const WindowManager& windowManager) {
		double xpos, ypos;
		glfwGetCursorPos(windowManager.getWindow(), &xpos, &ypos);
		return { xpos, ypos };
	}
};
