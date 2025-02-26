#pragma once
#include <string>
#include <iostream>
#include <memory>
#include "ShaderProgram.hpp"

class ShaderProgram;

// Uniform location wrapper
class UniLocation {

	public:

		UniLocation(const std::string& name, const std::shared_ptr<ShaderProgram>& program, bool isTemporary = false);
		UniLocation(const UniLocation& other, std::shared_ptr<ShaderProgram> replaceProgram = nullptr);

		void update(std::shared_ptr<ShaderProgram> program);
		int getLocation() const;
		const std::string& getName() const;

		bool operator==(const UniLocation& other) const;

	private:

		std::string name;
		int location;
		unsigned int programId;
		bool isTemporary;
};

// Hash function for UniLocation
template<>
struct std::hash<UniLocation> {
	std::size_t operator()(const UniLocation& location) const {
		return std::hash<std::string>()(location.getName());
	}
};
