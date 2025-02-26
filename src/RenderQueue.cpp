
#include "RenderQueue.hpp"

void RenderQueue::render() {
	Material::resetLastMaterial();
	for (auto& mesh : renderables) {
		mesh->draw(false, Material::BlendMode::ALPHA_CLIP, false, true);
	}
	Mesh::unbind();

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Material::resetLastMaterial();
	for (auto& mesh : renderables) {
		mesh->draw(false, Material::BlendMode::ALPHA_BLEND, false, true);
	}
	Mesh::unbind();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void RenderQueue::add(const std::shared_ptr<Mesh>& renderable) {
	this->renderables.push_back(renderable);
}

void RenderQueue::add(const std::list<std::shared_ptr<Mesh>>& renderables) {
	this->renderables.insert(this->renderables.end(), renderables.begin(), renderables.end());
}

void RenderQueue::remove(const std::shared_ptr<Mesh>& renderable) {
	renderables.remove(renderable);
}

void RenderQueue::clear() {
	this->renderables.clear();
}

const std::list<std::shared_ptr<Mesh>>& RenderQueue::getRenderables() const {
	return this->renderables;
}
