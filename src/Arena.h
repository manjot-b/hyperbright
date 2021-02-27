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
	using WallList = std::vector<std::shared_ptr<Model>>;

	Arena(
		std::shared_ptr<Model> wall,
		size_t length,
		size_t width);
	~Arena();

	void render(const Shader& shader) const;
	glm::vec2 isOnTile(const glm::vec3& coords) const;

	void setTileColor(const glm::vec2& tileCoords, const glm::vec4& color);
	void addWall(unsigned int row, unsigned int col, unsigned int width, unsigned int length);
	const WallList& getWalls() const;
	
private:
	class Tile {
	public:
		Tile(glm::mat4& modelMatrix, glm::vec4& color);
		void translate(const glm::vec3& trans);
		void setColor(const glm::vec4& color);

	private:
		glm::mat4& modelMatrix;
		glm::vec4& color;
	};

	std::shared_ptr<Model> instancedTile;
	std::shared_ptr<Model> instancedTileBorder;

	// Each tile/tile border is instanced, so we need to store all model matrices in one array.
	InstanceModelMatricesPtr tileModelMatrices;
	InstanceColorsPtr tileColors;

	using TileGrid = std::vector<std::vector<Tile>>;
	TileGrid tileGrid;

	std::shared_ptr<Model> wall;
	WallList walls;

	float tileWidth;
	float tileBorderWidth;	// This is the width of one edge of the border.
	float tileCollisionRadius;
	float wallWidth;
};
