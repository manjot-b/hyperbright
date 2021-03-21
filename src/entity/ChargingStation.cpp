#include "ChargingStation.h"

namespace hyperbright {
namespace entity {
ChargingStation::ChargingStation(const std::shared_ptr<openGLHelper::Shader>& shader) : IRenderable(shader),
	model(std::make_unique<model::Model>("rsc/models/transformer.obj", "station", shader, nullptr))
{
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

void ChargingStation::setTileCoords(const glm::vec2& tileCoords) { _tileCoords = tileCoords; }
const glm::vec2& ChargingStation::getTileCoords() const { return _tileCoords; }
}	// namespace entity
}	// namespace hyperbright