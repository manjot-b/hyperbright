#include "Arena.h"

Arena::Tile::Tile(const std::shared_ptr<Model> tile, const std::shared_ptr<Model> tileBorder) :
	tile(*tile), tileBorder(*tileBorder)
{

}

void Arena::Tile::render(const Shader& shader) const
{
	tile.render(shader);
	tileBorder.render(shader);
}

void Arena::Tile::translate(const glm::vec3& trans)
{
	tile.translate(trans);
	tileBorder.translate(trans);

	tile.update();
	tileBorder.update();
}

/*
 Construct an arena:

 Parameters:
	tile: Will copy the tile model.
	tileBorder: Will copy the tile border.
	rows: The number of tiles on the z-axis.
	cols: The number of tiles on the x-axis.
*/
Arena::Arena(const std::shared_ptr<Model> tile, const std::shared_ptr<Model> tileBorder, unsigned int rows, unsigned int cols) :
	tileGrid(rows)
{
	const BoundingBox& tileBox = tileBorder->getBoundingBox();
	glm::vec3 trans(0.f);
	trans.y = -tileBox.height / 2.f;	// Top of grid should be at y=0

	for (unsigned int row = 0; row < tileGrid.size(); row++)
	{
		tileGrid[row] = std::vector<Tile>(cols, Tile(tile, tileBorder));

		trans.z = -(rows * .5f) * tileBox.depth + (row * tileBox.depth) + tileBox.depth * 0.5;
		for (unsigned int col = 0; col < tileGrid[row].size(); col++)
		{
			trans.x = -(cols * .5f) * tileBox.width + (col * tileBox.width) + tileBox.width * .5f;
			tileGrid[row][col].translate(trans);
		}
	}
}

Arena::~Arena() {}

void Arena::render(const Shader& shader) const
{
	for (const auto& row : tileGrid)
	{
		for (const auto& tile : row)
		{
			tile.render(shader);
		}
	}
}