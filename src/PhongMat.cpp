#include "PhongMat.hpp"

std::shared_ptr<ShaderProgram> PhongMat::phongShaderProgram = nullptr;

std::shared_ptr<Texture> PhongMat::defaultDiffuseMap = nullptr;
std::shared_ptr<Texture> PhongMat::defaultNormalMap = nullptr;
std::shared_ptr<Texture> PhongMat::defaultSpecularMap = nullptr;
std::shared_ptr<Texture> PhongMat::defaultShininessMap = nullptr;
