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
	ShaderProgram(const Shader<GL_VERTEX_SHADER>& vertexShader, const Shader<GL_FRAGMENT_SHADER>& fragmentShader) {
		this->master = true;
		this->id = glCreateProgram();
		glAttachShader(this->id, vertexShader.getId());
		glAttachShader(this->id, fragmentShader.getId());
		glLinkProgram(this->id);

		int success;
		char infoLog[512];
		glGetProgramiv(this->id, GL_LINK_STATUS, &success);

		if (!success) {
			glGetProgramInfoLog(this->id, 512, NULL, infoLog);
			std::cout << "Error linking shader program\n" << infoLog << '\n';
		}
	}

	ShaderProgram(ShaderProgram&& other) {
		this->id = other.id;
		this->master = true;
		other.master = false;
	}

	unsigned int getId() const {
		return this->id;
	}

	const ShaderProgram& use() {
		glUseProgram(this->id);
		currentProgram = this;
		return *this;
	}

	static ShaderProgram* getCurrentProgram() {
		if (currentProgram == nullptr) {
			std::cout << "Warning: No shader program is currently in use\n";
		}
		return currentProgram;
	}

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

	~ShaderProgram() {
		if (master) {
			glDeleteProgram(this->id);
		}
	}

private:
	unsigned int id;
	static ShaderProgram* currentProgram;
	// If this is true, then this object is responsible for deleting the shader program
	bool master;
};

