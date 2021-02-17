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

void Arena::Tile::setColor(const glm::vec4& color)
{
	tile.setColor(color);
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
	tileGrid(rows), tileCollisionRadius(0.5f)
{
	const BoundingBox& tileBox = tileBorder->getBoundingBox();
	glm::vec3 trans(0.f);
	trans.y = -tileBox.height / 2.f;	// Top of grid should be at y=0

	for (unsigned int row = 0; row < tileGrid.size(); row++)
	{
		tileGrid[row] = std::vector<Tile>(cols, Tile(tile, tileBorder));

		trans.z = (rows * .5f) * tileBox.depth - (row * tileBox.depth) - tileBox.depth * 0.5;	// in OpenGL forward z points out of screen
		for (unsigned int col = 0; col < tileGrid[row].size(); col++)
		{
			trans.x = -(cols * .5f) * tileBox.width + (col * tileBox.width) + tileBox.width * .5f;
			tileGrid[row][col].translate(trans);
		}
	}

	tileWidth = tileBox.width;
	tileDepth = tileBox.depth;
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

/*
 * Returns the coordinates of the tile, if any, that the given coordinates are over.
 * Does not check the y-axis, only x and z. This function also uses the predefined radius
 * as a tolerance.
*/
std::optional<glm::vec2> Arena::isOnTile(const glm::vec3& coords) const
{
	unsigned int rows = tileGrid.size();
	unsigned int cols = tileGrid[0].size();

	int col = (coords.x + (cols * .5f) * tileWidth) / tileWidth;
	int row = ((rows * .5f) * tileWidth - coords.z) / tileWidth;

	if (col < 0 || col > cols - 1 || row < 0 || row > rows - 1)
	{
		// Make sure the column and row are within the bounds.
		return std::nullopt;
	}

	// TO-DO:
	// Make sure the coordinates are within the raidus tolerance.

	return glm::vec2(row, col);
}

void Arena::setTileColor(const glm::vec2& tileCoords, const glm::vec4& color)
{
	tileGrid[tileCoords.x][tileCoords.y].setColor(color);
}