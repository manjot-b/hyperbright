#include "ChargingStation.h"

namespace hyperbright {
namespace entity {
ChargingStation::ChargingStation(const std::shared_ptr<openGLHelper::Shader>& shader) : IRenderable(shader),
	model(std::make_unique<model::Model>("rsc/models/transformer.obj", "station", shader, nullptr))
{
}

void ChargingStation::render() const
{
	model->render();
}

void ChargingStation::setTileCoords(const glm::vec2& tileCoords) { _tileCoords = tileCoords; }
const glm::vec2& ChargingStation::getTileCoords() const { return _tileCoords; }
}	// namespace entity
}	// namespace hyperbright