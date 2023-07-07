#pragma once
#include "UniLocation.hpp"


UniLocation::UniLocation(const std::string& name, const std::shared_ptr<ShaderProgram>& program, bool isTemporary) : name(name), isTemporary(isTemporary) {
	if (program == nullptr || isTemporary) {
		if (isTemporary) {
			this->programId = program->getId();
		}
		else {
			this->programId = -1;
		}
		this->location = -1;
	}
	else {
		this->programId = program->getId();
		this->location = glGetUniformLocation(programId, name.c_str());
	}
}

UniLocation::UniLocation(const UniLocation& other, std::shared_ptr<ShaderProgram> replaceProgram) : name(other.name), location(other.location), programId(other.programId) {
	this->isTemporary = false;
	if (replaceProgram != nullptr) {
		this->programId = replaceProgram->getId();
		this->location = glGetUniformLocation(programId, name.c_str());
	}
	else if (other.isTemporary) {
		this->location = glGetUniformLocation(programId, name.c_str());
	}
}

void UniLocation::update(std::shared_ptr<ShaderProgram> program) {
	if (program != nullptr) {
		this->programId = ShaderProgram::getCurrentProgram()->getId();
		this->location = glGetUniformLocation(programId, name.c_str());
	}
}

int UniLocation::getLocation() const {
	int _location = location;
	if (this->programId != ShaderProgram::getCurrentProgram()->getId()) {
		_location = glGetUniformLocation(ShaderProgram::getCurrentProgram()->getId(), this->name.c_str());
		std::cout << "Warning: Uniform location \"" << this->name << "\" was queried from a different program than the assigned one\n";
	}
	return _location;
}

const std::string& UniLocation::getName() const {
	return name;
}

bool UniLocation::operator==(const UniLocation& other) const {
	return this->name == other.name;
}
