#pragma once

#include "render/Renderer.h"
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

private:
	std::unique_ptr<model::Model> model;
	glm::vec2 _tileCoords;
};
}	// namespace entity
}	// namespace hyperbright