#include "Arena.h"

Arena::Tile::Tile(const std::shared_ptr<Model> tile, const std::shared_ptr<Model> tileBorder) :
	tile(*tile), tileBorder(*tileBorder)
{

}

void Arena::Tile::draw(const Shader& shader) const
{
	tile.draw(shader);
	tileBorder.draw(shader);
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

	for (unsigned int row = 0; row < tileGrid.size(); row++)
	{
		tileGrid[row] = std::vector<Tile>(cols, Tile(tile, tileBorder));

		float zTrans = -(rows / 2.f) * tileBox.depth + (row * tileBox.depth);
		for (unsigned int col = 0; col < tileGrid[row].size(); col++)
		{
			float xTrans = -(cols / 2.f) * tileBox.width + (col * tileBox.width);
			tileGrid[row][col].translate(glm::vec3(xTrans, 0, zTrans));
		}
	}
}

Arena::~Arena() {}

void Arena::draw(const Shader& shader) const
{
	for (auto& row : tileGrid)
	{
		for (auto& tile : row)
		{
			tile.draw(shader);
		}
	}
}