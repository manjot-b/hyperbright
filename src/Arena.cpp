#include "Arena.h"

#include <glm/gtc/matrix_transform.hpp>


Arena::Tile::Tile(glm::mat4& modelMatrix, glm::vec4& color) :
	modelMatrix(modelMatrix), color(color), _hasWall(false)
{}

void Arena::Tile::translate(const glm::vec3& trans)
{
	modelMatrix = glm::translate(modelMatrix, trans);
}

void Arena::Tile::setColor(const glm::vec4& color)
{
	this->color = color;
}

bool Arena::Tile::hasWall() const { return _hasWall; }

/*
 Construct an arena:

 Parameters:
	wall: This wall model will be modified several times before being copied.
	rows: The number of tiles on the z-axis.
	cols: The number of tiles on the x-axis.
*/
//bool AiArenaRepresentation;

Arena::Arena(size_t rows, size_t cols) :
	tileModelMatrices( std::make_shared<std::vector<glm::mat4>>( rows * cols, glm::mat4(1.f) )),
	tileColors( std::make_shared<std::vector<glm::vec4>>(rows * cols, glm::vec4(.3f, .3f, .3f, 1.f)) ),
	tileGrid(rows), tileCollisionRadius(0.5f)
{

	instancedTile = std::make_shared<Model>("rsc/models/tile.obj", "tile", nullptr, std::nullopt);
	instancedTileBorder = std::make_shared<Model>("rsc/models/tile_edge.obj", "tile", nullptr, std::nullopt);

	const BoundingBox& tileBox = instancedTileBorder->getBoundingBox();
	glm::vec3 trans(0.f);
	trans.y = -tileBox.height / 2.f;	// Top of grid should be at y=0

	for (size_t row = 0; row < rows; row++)
	{
		tileGrid[row] = std::vector<Tile>();
		trans.z = (rows * .5f) * tileBox.depth - (row * tileBox.depth) - tileBox.depth * 0.5;	// in OpenGL forward z points out of screen
		
		for (size_t col = 0; col < cols; col++)
		{
			unsigned int instanceIdx = row * rows + col;
			tileGrid[row].push_back(Tile((*tileModelMatrices)[instanceIdx], (*tileColors)[instanceIdx]));
			trans.x = -(cols * .5f) * tileBox.width + (col * tileBox.width) + tileBox.width * .5f;
			tileGrid[row][col].translate(trans);
		}
	}

	instancedTile->setInstanceModelMatrices(tileModelMatrices);
	instancedTile->setInstanceColors(tileColors);
	instancedTileBorder->setInstanceModelMatrices(tileModelMatrices);

	tileWidth = instancedTile->getBoundingBox().width;
	tileBorderWidth = (tileBox.width - instancedTile->getBoundingBox().width) * 0.5f;	// width of only one edge.
}

Arena::~Arena() {}

void Arena::render(const Shader& shader) const
{
	instancedTile->setInstanceColors(tileColors);
	instancedTile->render(shader);
	instancedTileBorder->render(shader);

	for (const auto& wall : walls)
	{
		wall->render(shader);
	}
}

/*
 * Returns the coordinates of the tile, if any, that the given coordinates are over.
 * Does not check the y-axis, only x and z. This function also uses the predefined radius
 * as a tolerance.
*/
glm::vec2 Arena::isOnTile(const glm::vec3& coords) const
{
	unsigned int rows = tileGrid.size();
	unsigned int cols = tileGrid[0].size();

	float fullTileWidth = tileWidth + 2.f * tileBorderWidth;
	int col = (coords.x + (cols * .5f) * fullTileWidth) / fullTileWidth;
	int row = ((rows * .5f) * fullTileWidth - coords.z) / fullTileWidth;

	if (col < 0 || col > cols - 1 || row < 0 || row > rows - 1)
	{
		// Make sure the column and row are within the bounds.
		return glm::vec2(-1, -1);
	}

	// TO-DO:
	// Make sure the coordinates are within the raidus tolerance.

	return glm::vec2(row, col);
}

void Arena::setTileColor(const glm::vec2& tileCoords, const glm::vec4& color)
{
	tileGrid[tileCoords.x][tileCoords.y].setColor(color);
}

void Arena::addWall(unsigned int row, unsigned int col, unsigned int width, unsigned int length)
{
	walls.push_back(std::make_unique<Model>("rsc/models/wall.obj", "walal" + walls.size(), nullptr, std::nullopt));
	auto& wall = walls.back();
	float wallWidth = wall->getBoundingBox().width;

	glm::vec2 scale = glm::vec2(
		(width * tileWidth + 2 * width * tileBorderWidth),
		(length * tileWidth + 2 * length * tileBorderWidth)
	) / wallWidth;

	wall->scale(glm::vec3(scale.x, 1, scale.y));
	const BoundingBox& wallBox = wall->getBoundingBox();

	glm::vec2 currentPos = glm::vec2(wallBox.x, -wallBox.z) * scale;	// the front left of the box, not the origin.

	unsigned int rows = tileGrid.size();
	unsigned int cols = tileGrid[0].size();
	float fullTileWidth = tileWidth + 2.f * tileBorderWidth;

	glm::vec2 targetPos = glm::vec2(
		-(cols * .5f) * fullTileWidth + (col * fullTileWidth),
		(rows * .5f) * fullTileWidth - (row * fullTileWidth)
	);

	float yTrans = wall->getBoundingBox().height * 0.5f;	// Base of wall at y=0
	glm::vec3 trans(targetPos.x - currentPos.x, yTrans, targetPos.y - currentPos.y);

	wall->translate(trans);
	wall->update();

	for (unsigned int r = row; r < row + length; r++)
	{
		for (unsigned int c = col; c < col + width; c++)
		{
			tileGrid[r][c]._hasWall = true;
		}
	}
}

const Arena::WallList& Arena::getWalls() const { return walls; }