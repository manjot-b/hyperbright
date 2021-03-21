#pragma once

#include "render/Renderer.h"

#include <array>
#include <vector>

#include "model/Model.h"

namespace hyperbright {
namespace entity {

class Arena;

class ChargingStation : public render::Renderer::IRenderable
{
	friend class Arena;
public:
	ChargingStation(const std::shared_ptr<openGLHelper::Shader>& shader);
	void render() const;

	void setTileCoords(const glm::vec2& tileCoords);
	const glm::vec2& getTileCoords() const;
	void animate(float time);

private:
	std::unique_ptr<model::Model> model;
	glm::vec2 _tileCoords;
	std::array<glm::vec3, 2> animationColors;
	std::vector<unsigned int> poleMeshIndices;
};
}	// namespace entity
}	// namespace hyperbright