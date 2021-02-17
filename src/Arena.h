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
	Arena(const std::shared_ptr<Model> tile, const std::shared_ptr<Model> tileBorder, unsigned int length, unsigned int width);
	~Arena();

	void render(const Shader& shader) const;
	std::optional<glm::vec2> isOnTile(const glm::vec3& coords) const;

	void setTileColor(const glm::vec2& tileCoords, const glm::vec4& color);
	
private:
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

	float tileWidth;
	float tileBorderWidth;	// This is the width of one edge of the border.
	float tileCollisionRadius;
};
