#pragma once

#include <glm/glm.hpp>

#include <optional>
#include <vector>
#include <memory>

#include "entity/ChargingStation.h"
#include "model/Model.h"
#include "render/Renderer.h"
#include "opengl-helper/Shader.h"
#include "engine/TeamStats.h"

namespace hyperbright {
namespace entity {
class Arena : public render::Renderer::IRenderable
{
public:
	using WallList = std::vector<std::unique_ptr<model::Model>>;
	using ChargingStationList = std::vector<std::unique_ptr<entity::ChargingStation>>;

	/*
	 Orientation relative to world coords.
	*/
	enum class Orientation {
		POS_X = 0,
		NEG_Z,
		NEG_X,
		POS_Z,
		
	};

	Arena(size_t length, size_t width, const std::shared_ptr<openGLHelper::Shader>& shader, float tileScale = 5);
	~Arena();

	void render() const;
	std::optional<glm::vec2> isOnTile(const glm::vec3& coords) const;
	glm::vec3 getTilePos(const glm::vec2& coords) const;
	std::optional<engine::teamStats::Teams> getTeamOnTile(const glm::vec2& coords) const;

	void setTileTeam(const glm::vec2& tileCoords, engine::teamStats::Teams team);
	void addWall(unsigned int col, unsigned int row, unsigned int width, unsigned int length);
	const WallList& getWalls() const;

	void addChargingStation(unsigned int col, unsigned int row, Orientation orientation);
	bool tileHasChargingStation(const glm::vec2& tileCoords);

	std::vector<std::vector<bool>> getAiArenaRepresentation();
private:
	class Tile {
		friend class Arena;

	public:
		Tile(glm::mat4& modelMatrix, glm::vec4& color);
		void translate(const glm::vec3& trans);
		void scale(float scale);
		void setColor(const glm::vec4& color);
		bool hasWall() const;
		bool hasChargingStation() const;

	private:
		glm::mat4& modelMatrix;
		glm::vec4& color;
		bool _hasWall;
		bool _hasChargingStation;
		std::optional<engine::teamStats::Teams> team;	// tile may not have a team.
	};

	std::shared_ptr<model::Model> instancedTile;
	std::shared_ptr<model::Model> instancedTileBorder;

	// Each tile/tile border is instanced, so we need to store all model matrices in one array.
	model::InstanceModelMatricesPtr tileModelMatrices;
	model::InstanceColorsPtr tileColors;

	using TileGrid = std::vector<std::vector<Tile>>;
	TileGrid tileGrid;
	WallList walls;
	ChargingStationList chargingStations;

	float tileWidth;
	float tileBorderWidth;	// This is the width of one edge of the border.
	float tileScale;
	float tileCollisionRadius;
};
}	// namespace entity
}	// namespace hyperbright