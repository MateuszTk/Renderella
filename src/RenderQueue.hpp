#pragma once
#include <memory>
#include <list>
#include "Mesh.hpp"

class RenderQueue {
public:
	RenderQueue() {

	}

	void render() {
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

	void add(const std::shared_ptr<Mesh>& renderable) {
		this->renderables.push_back(renderable);
	}

	void add(const std::list<std::shared_ptr<Mesh>>& renderables) {
		this->renderables.insert(this->renderables.end(), renderables.begin(), renderables.end());
	}

	void add(std::list<Mesh>&& renderables) {
		for (auto& renderable : renderables) {
			this->renderables.push_back(std::make_shared<Mesh>(std::move(renderable)));
		}
	}

	void remove(const std::shared_ptr<Mesh>& renderable) {
		renderables.remove(renderable);
	}

	void clear() {
		this->renderables.clear();
	}

	const std::list<std::shared_ptr<Mesh>>& getRenderables() const {
		return this->renderables;
	}

private:
	std::list<std::shared_ptr<Mesh>> renderables;
};
