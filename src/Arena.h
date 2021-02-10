#pragma once

#include <glm/glm.hpp>

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
	
private:
	class Tile : public Renderer::IRenderable {
	public:
		Tile(const std::shared_ptr<Model> tile, const std::shared_ptr<Model> tileBorder);
		void render(const Shader& shader) const;
		void translate(const glm::vec3& trans);

	private:
		Model tile;
		Model tileBorder;
	};

	using TileGrid = std::vector<std::vector<Tile>>;
	TileGrid tileGrid;
};
