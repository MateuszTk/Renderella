#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include "Camera.hpp"

class Light {

	public:

		enum class Type {
			POINT = 0,
			DIRECTIONAL = 1
		};

		Light(Type type, const glm::vec3& pos, const glm::vec3& color, float intensity = 1.0f);

		virtual ~Light();

		const glm::vec3 getColor() const;
		void setColor(const glm::vec3& color);

		float getIntensity() const;
		void setIntensity(float intensity);

		const glm::vec3 getPosition() const;
		void setPosition(const glm::vec3& position);

		Type getType() const;
		void setType(Type type);

		static Light** getLights();
		static glm::vec4* getLightPositions();
		static glm::vec3* getLightColors();
		static glm::vec3* getLightDirections();
		static glm::mat4* getLightSpaceMatrices();
		static int getUsedLightsCnt();
		static int getMaxLights();
		static int getMaxLightSpaceMatrices();

	protected:

		glm::vec3 position;
		glm::vec3 color;
		Type type;
		float intensity;
		const static int maxLights = 8;
		const static int maxLightSpaceMatrices = 8;
		static int usedLights;
		static Light* lights[maxLights];

		// the w component of the light position is used to store the light type
		static glm::vec4 lightPositions[maxLights];

		static glm::vec3 lightColors[maxLights];

		static glm::vec3 lightDirections[maxLights];

		static glm::mat4 lightSpaceMatrices[maxLightSpaceMatrices];
		// maps the type of the light to the number of light space matrices it uses
		static int lightSpaceMatrixCount[2];

		virtual void updateLists();
};
