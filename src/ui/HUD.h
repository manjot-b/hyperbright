#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <FTGL/ftgl.h>

namespace hyperbright {
namespace ui {

class HUD
{
public:
	HUD(float s, float e);
	void drawHUD();

private:
	FTGLPixmapFont font;
	float defaultFontSize;
	unsigned int width, height;
	float speed;
	float energy;

	void updateWindowAndFontSize();
};

}	// namespace ui
}	// namespace hyperbright