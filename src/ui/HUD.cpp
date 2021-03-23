#include "HUD.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include "render/Renderer.h"

namespace hyperbright {
namespace ui {

HUD::HUD(float s, float e) : font("rsc/fonts/neon_pixel-7.ttf"), defaultFontSize(150.f), speed(s), energy(e) {}

void HUD::updateWindowAndFontSize()
{
	render::Renderer::getInstance().getWindowSize(width, height);
	//defaultFontSize = width * 0.05;
}

void HUD::drawHUD() {
	updateWindowAndFontSize();
	float scale = (width * 0.125) / defaultFontSize;
	float xCord = (float)width - (3 * 63 * scale);
	float yCord = 20 * scale;

	//speedometer
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_BIAS, 0);
	glPixelTransferf(GL_BLUE_BIAS, 0);
	double realSpeed = std::max(round(speed * 4), 0.f);

	char str[100];
	sprintf(str, "%d", (int)realSpeed);
	font.FaceSize(scale * defaultFontSize);
	font.Render(str, -1, FTPoint(xCord, yCord, 0));
	glPopAttrib();


	//energy
	int numOfBar = (int)(energy * 25);
	glBegin(GL_QUADS);
	glColor3f(0, 0.71, 1);
	for (float i = -0.995; i < (-1 + 0.02 * numOfBar); i += (0.005 + 0.015)) {
		glVertex2f(i, -0.95);
		glVertex2f(i, -0.8);
		glVertex2f(i + 0.015, -0.8);
		glVertex2f(i + 0.015, -0.95);
	}
	glEnd();
}


}	// namespace ui
}	// namespace hyperbright