#include "ChargingStation.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace hyperbright {
namespace entity {
ChargingStation::ChargingStation(const std::shared_ptr<openGLHelper::Shader>& shader) :
	IRenderable(shader),
	model(std::make_unique<model::Model>("rsc/models/transformer.obj", "station", shader, nullptr))
{
	setTriggerType(physics::IPhysical::TriggerType::CHARGING_STATION);

	for (unsigned int i = 0; i < model->getMeshes().size(); i++)
	{
		if (model->getMeshes()[i]->getName() == "shiny_metal")
		{
			animationColors[0] = model->getMeshes()[i]->material.color;
			poleMeshIndices.push_back(i);
		}
	}
	animationColors[1] = glm::vec3(.9f, .41f, .71f);
}

void ChargingStation::render() const
{
	model->render();
}

void ChargingStation::renderShadow(const std::shared_ptr<openGLHelper::Shader>& shadowShader) const
{
	model->renderShadow(shadowShader);
}

void ChargingStation::renderMiniMap() const
{
	glm::mat4 original = model->getModelMatrix();
	glm::mat4 scaled = glm::scale(original, glm::vec3(5.f));
	model->setModelMatrix(scaled);
	model->render();
	model->setModelMatrix(original);
}

void ChargingStation::animate(float time)
{
	const float speed = 7.5f;
	float mix = (cos(time * speed) + 1.f) * 0.5f;

	glm::vec3 mixedColor1 = mix * animationColors[0] + (1 - mix) * animationColors[1];
	glm::vec3 mixedColor2 = mix * animationColors[1] + (1 - mix) * animationColors[0];

	for (unsigned int i = 0; i < poleMeshIndices.size(); i++)
	{
		unsigned int idx = poleMeshIndices[i];
		if (i % 2)
			model->getMeshes()[idx]->material.color = glm::vec4(mixedColor1, 1.f);
		else
			model->getMeshes()[idx]->material.color = glm::vec4(mixedColor2, 1.f);
	}
}

const glm::vec3& ChargingStation::getDimensions() const { return dimensions; }

void ChargingStation::setModelMatrix(const glm::mat4& modelMat) { model->setModelMatrix(modelMat); }
void ChargingStation::setPosition(const glm::vec3& position) { model->setPosition(position); }

void ChargingStation::setTileCoords(const glm::vec2& tileCoords) { _tileCoords = tileCoords; }
const glm::vec2& ChargingStation::getTileCoords() const { return _tileCoords; }

void ChargingStation::setWorldCoords(const glm::vec3& worldCoords) { _worldCoords = worldCoords; }
const glm::vec3& ChargingStation::getWorldCoords() const { return _worldCoords; }

}	// namespace entity
}	// namespace hyperbright