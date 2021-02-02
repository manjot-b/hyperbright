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

	for (auto& row : tileGrid)
	{
		row = std::vector<Tile>(cols, Tile(tile, tileBorder));
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