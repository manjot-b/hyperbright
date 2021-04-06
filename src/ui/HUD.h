#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <FTGL/ftgl.h>

#include "entity/Arena.h"
#include "entity/Vehicle.h"
#include "entity/Pickup.h"
#include "opengl-helper/FrameBuffer.h"
#include "opengl-helper/Quad.h"
#include "opengl-helper/Texture.h"

namespace hyperbright {
namespace ui {

class HUD
{
public:
	HUD(std::shared_ptr<entity::Vehicle> v, const entity::Arena& arena, float& roundTimer);
	void drawHUD();
	void updateTime(float time);
	void update(float s, float e);
	void preRenderMiniMap();

	const glm::vec3& getMiniMapCameraPos();
	const glm::mat4& getMiniMapCameraView();
	const glm::mat4& getMiniMapOrtho();

private:
	FTGLPixmapFont font;
	float defaultFontSize;
	unsigned int width, height;
	float speed;
	float energy;
	float &roundTimer;
	std::shared_ptr<entity::Vehicle> player;
	std::shared_ptr<entity::Pickup> pickup;

	std::shared_ptr<openGLHelper::Texture> miniMapTexture;
	std::unique_ptr<openGLHelper::FrameBuffer> miniMapBuffer;
	std::shared_ptr<openGLHelper::Texture> timerTexture;
	std::shared_ptr<openGLHelper::Shader> quadShader;
	std::unique_ptr<openGLHelper::Quad> quad;
	std::unique_ptr<openGLHelper::Quad> timerQuad;
	std::unique_ptr<openGLHelper::Quad> emp;
	std::unique_ptr<openGLHelper::Quad> zap;
	std::unique_ptr<openGLHelper::Quad> speedz;
	std::unique_ptr<openGLHelper::Quad> slowtrap;
	std::unique_ptr<openGLHelper::Quad> syphon;
	std::unique_ptr<openGLHelper::Quad> slowed;
	std::unique_ptr<openGLHelper::Quad> zapped;
	std::unique_ptr<openGLHelper::Quad> syphonOn;


	glm::vec3 miniMapPos;
	glm::mat4 miniMapOrtho;
	glm::mat4 miniMapView;

	void updateWindowAndFontSize();
};

}	// namespace ui
}	// namespace hyperbright