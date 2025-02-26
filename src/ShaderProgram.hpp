#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include <iostream>
#include <string>
#include <vector>
#include "UniLocation.hpp"

class UniLocation;

class ShaderProgram {

	public:

		ShaderProgram(const Shader<GL_VERTEX_SHADER>& vertexShader, const Shader<GL_FRAGMENT_SHADER>& fragmentShader);
		ShaderProgram(const Shader<GL_VERTEX_SHADER>& vertexShader, const Shader<GL_FRAGMENT_SHADER>& fragmentShader, const Shader<GL_GEOMETRY_SHADER>& geometryShader);
		ShaderProgram(ShaderProgram&& other);
		~ShaderProgram();

		unsigned int getId() const;

		const ShaderProgram& use();

		static ShaderProgram* getCurrentProgram();

		void setInt(const UniLocation& location, int value);
		void setFloat(const UniLocation& location, float value);
		void setVec2(const UniLocation& location, const glm::vec2& value);
		void setVec3(const UniLocation& location, const glm::vec3& value);
		void setVec3s(const UniLocation& location, std::vector<glm::vec3> values);
		void setVec3s(const UniLocation& location, glm::vec3* values, int size);
		void setVec4(const UniLocation& location, const glm::vec4& value);
		void setVec4s(const UniLocation& location, glm::vec4* values, int size);
		void setMat3(const UniLocation& location, const glm::mat3& value);
		void setMat4(const UniLocation& location, const glm::mat4& value);
		void setMat4s(const UniLocation& location, glm::mat4* values, int size);

	private:

		unsigned int id;
		static ShaderProgram* currentProgram;
		// If this is true, then this object is responsible for deleting the shader program
		bool master;

};

