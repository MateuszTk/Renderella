#pragma once
#include <memory>
#include <list>
#include "Mesh.hpp"

class RenderQueue {

public:

	RenderQueue() = default;

	void render();

	void add(const std::shared_ptr<Mesh>& renderable);
	void add(const std::list<std::shared_ptr<Mesh>>& renderables);

	void remove(const std::shared_ptr<Mesh>& renderable);

	void clear();

	const std::list<std::shared_ptr<Mesh>>& getRenderables() const;

private:
	std::list<std::shared_ptr<Mesh>> renderables;
};
