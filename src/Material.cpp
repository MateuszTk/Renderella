#include "Material.hpp"

std::shared_ptr<Material> Material::overrideMaterial = nullptr;
Material* Material::lastMaterial = nullptr;
