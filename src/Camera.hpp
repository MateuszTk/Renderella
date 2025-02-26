#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "WindowManager.hpp"
#include <vector>

class Camera {

	public:

		enum class ProjectionType {
			PERSPECTIVE,
			ORTHOGRAPHIC
		};

		Camera(ProjectionType projType, float aspectRatio, bool controllable = false, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& direction = glm::vec3(0.0f, 0.0f, 1.0f), float fov = 65.0f, float farPlane = 20.0f, float nearPlane = 0.4f);
		~Camera();

		const glm::vec3& getPosition() const;
		const glm::vec3& getDirection() const;

		void setPosition(const glm::vec3& position);
		void setDirection(const glm::vec3& direction);

		void setAspectRatio(float aspectRatio);
		float getAspectRatio() const;

		void update(const WindowManager& windowManager);

		virtual void use(bool multiLayer = false);

		const glm::mat4& getCameraMatrix();
		const glm::mat4& getViewMatrix();
		const glm::mat4& getProjectionMatrix();

		static Camera* getActiveCamera();
		static std::vector<Camera*> getActiveCameras();

		void setFov(float fov);
		float getFov() const;
		void setClipplingPlanes(float nearPlane, float farPlane);
		float getNearPlane() const;
		float getFarPlane() const;

		void setSpeed(float speed);
		float getSpeed() const;
		void setSensitivity(float sensitivity);
		float getSensitivity() const;
		bool isControllable() const;
		void setControllable(bool controllable);

		bool getIsLight() const;
		void setIsLight(bool isLight);

	protected:

		glm::vec3 position;	
		glm::mat4 cameraMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		static std::vector<Camera*> activeCameras;
		glm::vec3 direction;
		ProjectionType projType;

		float aspectRatio;
		float fov;
		float nearPlane;
		float farPlane;

		bool controllable = true;
		float speed = 4.0f;
		float sensitivity = 0.1f;

		bool isLight = false;

		//variables for mouse and keyboard input
		glm::vec2 lastCursorPos = glm::vec2(0.0f);
		glm::vec2 angle = glm::vec2(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
	
		const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	
		void processInput(const WindowManager& windowManager);

		void updateMatrix();

		glm::vec2 getCursorsPosition(const WindowManager& windowManager);
};
