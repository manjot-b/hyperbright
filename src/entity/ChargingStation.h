#pragma once

#include "render/IRenderable.h"

#include <array>
#include <vector>

#include "model/Model.h"
#include "physics/Interface.h"

namespace hyperbright {
namespace entity {

class Arena;

class ChargingStation : public render::IRenderable, public physics::IPhysical
{
	friend class Arena;
public:
	ChargingStation(const std::shared_ptr<openGLHelper::Shader>& shader);
	void render() const;
	void renderShadow(const std::shared_ptr<openGLHelper::Shader>& shadowShader) const;

	void setTileCoords(const glm::vec2& tileCoords);
	const glm::vec2& getTileCoords() const;
	void setWorldCoords(const glm::vec3& worldCoords);
	const glm::vec3& getWorldCoords() const;

	void setModelMatrix(const glm::mat4& modelMat);
	void setPosition(const glm::vec3& position);

	void animate(float time);
	const glm::vec3& getDimensions() const;

private:
	std::unique_ptr<model::Model> model;
	glm::vec2 _tileCoords;
	glm::vec3 _worldCoords;
	std::array<glm::vec3, 2> animationColors;
	std::vector<unsigned int> poleMeshIndices;
	glm::vec3 dimensions;
};
}	// namespace entity
}	// namespace hyperbright