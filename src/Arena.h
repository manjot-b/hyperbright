#pragma once

#include <glm/glm.hpp>

#include <optional>
#include <vector>
#include <memory>

#include "Model.h"
#include "Renderer.h"
#include "Shader.h"

class Arena : public Renderer::IRenderable
{
public:
	Arena(const std::shared_ptr<Model> tile,
		const std::shared_ptr<Model> tileBorder,
		std::shared_ptr<Model> wall,
		unsigned int length,
		unsigned int width);
	~Arena();

	void render(const Shader& shader) const;
	std::optional<glm::vec2> isOnTile(const glm::vec3& coords) const;

	void setTileColor(const glm::vec2& tileCoords, const glm::vec4& color);
	
private:
	/*
	 * The direction a wall extends in from its starting point.
	 */
	enum class EXTENSION {
		X_AXIS,
		Z_AXIS
	};
	class Tile : public Renderer::IRenderable {
	public:
		Tile(const std::shared_ptr<Model> tile, const std::shared_ptr<Model> tileBorder);
		void render(const Shader& shader) const;
		void translate(const glm::vec3& trans);
		void setColor(const glm::vec4& color);

	private:
		Model tile;
		Model tileBorder;
	};

	using TileGrid = std::vector<std::vector<Tile>>;
	TileGrid tileGrid;

	using WallList = std::vector<std::unique_ptr<Model>>;
	WallList walls;

	float tileWidth;
	float tileBorderWidth;	// This is the width of one edge of the border.
	float tileCollisionRadius;
	float wallWidth;

	void addWall(std::shared_ptr<Model>& wall, unsigned int row, unsigned int col, EXTENSION direction, unsigned int cellsCovered);
};
