#include "Arena.h"

#include <glm/gtc/matrix_transform.hpp>

namespace hyperbright {
namespace entity {
Arena::Tile::Tile(glm::mat4& modelMatrix, glm::vec4& color) :
	modelMatrix(modelMatrix), color(color), _hasWall(false), _hasChargingStation(false), isTrap(false), team(std::nullopt)
{}

void Arena::Tile::translate(const glm::vec3& trans)
{
	modelMatrix = glm::translate(modelMatrix, trans);
}

void Arena::Tile::scale(float scale)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
}

void Arena::Tile::setColor(const glm::vec4& color)
{
	this->color = color;
}

bool Arena::Tile::hasWall() const { return _hasWall; }

bool Arena::Tile::hasChargingStation() const { return _hasChargingStation; }

/*
 Construct an arena:

 Parameters:
	wall: This wall model will be modified several times before being copied.
	rows: The number of tiles on the z-axis.
	cols: The number of tiles on the x-axis.
*/
//bool AiArenaRepresentation;

Arena::Arena(size_t rows, size_t cols, const std::shared_ptr<openGLHelper::Shader>& shader, float tileScale) : IRenderable(shader),
	tileModelMatrices( std::make_shared<std::vector<glm::mat4>>( rows * cols, glm::mat4(1.f) )),
	tileBaseColor(.3f, .3f, .3f, 1.f),
	tileColors( std::make_shared<std::vector<glm::vec4>>(rows * cols, tileBaseColor) ),
	tileGrid(rows), tileScale(tileScale), tileCollisionRadius(0.5f)
{

	instancedTile = std::make_shared<model::Model>("rsc/models/tile.obj", "tile", shader, nullptr);
	instancedTileBorder = std::make_shared<model::Model>("rsc/models/tile_edge.obj", "tile", shader, nullptr);

	const model::BoundingBox& tileBox = instancedTileBorder->getBoundingBox();
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

			// Ordering matters.
			tileGrid[row][col].scale(tileScale);
			tileGrid[row][col].translate(trans);
			tileGrid[row][col].isTrap = false;
		}
	}

	instancedTile->setInstanceModelMatrices(tileModelMatrices);
	instancedTile->setInstanceColors(tileColors);
	instancedTileBorder->setInstanceModelMatrices(tileModelMatrices);

	tileWidth = instancedTile->getBoundingBox().width * tileScale;
	tileBorderWidth = (tileBox.width - instancedTile->getBoundingBox().width) * 0.5f * tileScale;	// width of only one edge.
}

Arena::~Arena() {}

void Arena::render() const
{
	instancedTile->setInstanceColors(tileColors);
	instancedTile->render();
	instancedTileBorder->render();

	for (const auto& wall : walls)
		wall->render();

	for (const auto& station : chargingStations)
		station->render();
}

void Arena::renderShadow(const std::shared_ptr<openGLHelper::Shader>& shadowShader) const
{
	instancedTile->renderShadow(shadowShader);
	instancedTileBorder->renderShadow(shadowShader);

	for (const auto& wall : walls)
		wall->renderShadow(shadowShader);

	for (const auto& station : chargingStations)
		station->renderShadow(shadowShader);
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

	float fullTileWidth = tileWidth + 2.f * tileBorderWidth;
	int col = (coords.x + (cols * .5f) * fullTileWidth) / fullTileWidth;
	int row = ((rows * .5f) * fullTileWidth - coords.z) / fullTileWidth;

	if (col < 0 || col > cols - 1 || row < 0 || row > rows - 1)
	{
		// Make sure the column and row are within the bounds.
		return std::nullopt;
	}

	// TO-DO:
	// Make sure the coordinates are within the raidus tolerance.

	return glm::vec2(row, col);
}

void Arena::setTileTeam(const glm::vec2& tileCoords, std::optional<engine::teamStats::Teams> team)
{
	glm::vec4 color = team ? engine::teamStats::colors.at(*team) : tileBaseColor;
	tileGrid[tileCoords.x][tileCoords.y].setColor(color);
	tileGrid[tileCoords.x][tileCoords.y].team = team;
}

glm::vec3 Arena::getTilePos(const glm::vec2& coords) const
{
	return tileGrid[coords.x][coords.y].modelMatrix[3];
}

std::optional<engine::teamStats::Teams> Arena::getTeamOnTile(const glm::vec2& coords) const
{
	return tileGrid[coords.x][coords.y].team;
}

void Arena::addWall(unsigned int col, unsigned int row, unsigned int width, unsigned int length)
{
	walls.push_back(std::make_unique<model::Model>("rsc/models/wall.obj", "wall" + walls.size(), _shader, nullptr));
	auto& wall = walls.back();
	float wallWidth = wall->getBoundingBox().width;

	glm::vec2 scale = glm::vec2(
		(width * tileWidth + 2 * width * tileBorderWidth),
		(length * tileWidth + 2 * length * tileBorderWidth)
	) / wallWidth;

	wall->scale(glm::vec3(scale.x, 1, scale.y));
	const model::BoundingBox& wallBox = wall->getBoundingBox();

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

void Arena::addChargingStation(unsigned int col, unsigned int row, Orientation orientation)
{
	chargingStations.push_back(std::make_unique<entity::ChargingStation>(_shader));

	auto& station = chargingStations.back();
	auto& boundingBox = station->model->getBoundingBox();

	unsigned int rows = tileGrid.size();
	unsigned int cols = tileGrid[0].size();
	float fullTileWidth = tileWidth + 2.f * tileBorderWidth;

	glm::vec3 trans = glm::vec3(
		-(cols * .5f) * fullTileWidth + (col * fullTileWidth) + fullTileWidth * .5f,
		0.f,
		(rows * .5f) * fullTileWidth - (row * fullTileWidth) - fullTileWidth * .5f
	);

	float rot = glm::radians(static_cast<int>(orientation) * 90.f);

	station->model->translate(trans);
	station->model->rotate(glm::vec3(.0f, rot, .0f));
	station->model->update();
	station->setTileCoords(glm::vec2(row, col));
	station->setWorldCoords(trans);

	switch (orientation)
	{
	case Orientation::NEG_X:
	case Orientation::POS_X:
		station->dimensions = glm::vec3(boundingBox.width, boundingBox.height, boundingBox.depth);
		break;
	case Orientation::NEG_Z:
	case Orientation::POS_Z:
		station->dimensions = glm::vec3(boundingBox.depth, boundingBox.height, boundingBox.width);
		break;
	}

	tileGrid[row][col]._hasChargingStation = true;
}

void Arena::placeTrap(glm::vec2 tileCoords) {

	tileGrid[tileCoords.x][tileCoords.y].setTrap();

	// make the color slightly darker
	glm::vec4 color = tileGrid[tileCoords.x][tileCoords.y].color;
	color = glm::vec4(glm::vec3(color) * 0.5f, 1.f);
	tileGrid[tileCoords.x][tileCoords.y].setColor(color);
}

bool Arena::isTrap(glm::vec2 tileCoords) {
	return tileGrid[tileCoords.x][tileCoords.y].isTrap;
	//piss titties
}

void Arena::removeTrap(glm::vec2 tileCoords) {

	tileGrid[tileCoords.x][tileCoords.y].removeTrap();
	
	// set the color back to the original color.
	std::optional<engine::teamStats::Teams> team = tileGrid[tileCoords.x][tileCoords.y].team;
	glm::vec4 color = team ? engine::teamStats::colors.at(*team) : tileBaseColor;

	tileGrid[tileCoords.x][tileCoords.y].setColor(color);
}

void Arena::animateChargingStations(float time)
{
	for (auto& station : chargingStations)
		station->animate(time);
}

bool Arena::tileHasChargingStation(const glm::vec2& tileCoords)
{
	return tileGrid[tileCoords.x][tileCoords.y].hasChargingStation();
}

const Arena::WallList& Arena::getWalls() const { return walls; }
const Arena::ChargingStationList& Arena::getChargingStations() const { return chargingStations; }

//MAY BE ADDING IN WRONG ORDER CHECK LATER
std::vector<std::vector<bool>> Arena::getAiArenaRepresentation() {
	std::vector<std::vector<bool>> arenaRep;
	for (int i = 0; i < tileGrid.size() ; i++) {
		std::vector<bool> nextCol;		
		for (int j = 0; j < tileGrid.at(i).size(); j++) {
			nextCol.emplace_back(tileGrid[i][j].hasWall());
		}
		arenaRep.emplace_back(nextCol);
	}
	return arenaRep;
}

glm::vec2 Arena::getArenaSize() const { return glm::vec2(tileGrid.size(), tileGrid[0].size()); }
float Arena::getTileWidth() const { return tileWidth + 2 * tileBorderWidth; }

}	// namespace entity
}	// namespace hyperbright