#define _USE_MATH_DEFINES
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
	float yCord = 45 * scale;

	//Speedometer
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_BIAS, -0.7);
	glPixelTransferf(GL_BLUE_BIAS, -1);
	double realSpeed = std::max(round(speed * 5), 0.f);

	char speedStr[100];
	sprintf(speedStr, "%d", (int)realSpeed);
	font.FaceSize(scale * defaultFontSize);
	font.Render(speedStr, -1, FTPoint(xCord, yCord, 0));
	font.FaceSize(scale * defaultFontSize / 3);
	font.Render("KPH", -1, FTPoint(width - (3 * 21 * scale), 10* scale, 0));
	glPopAttrib();

	/*
	float max = M_PI - (speed / 29) * (M_PI / 2);
	float bigRadius = 0.9;
	float smallRadius = 0.7;
	float bigWidth = (height * bigRadius) / width;
	float smallWidth = (height * smallRadius) / width;

	glBegin(GL_QUADS);
	glColor3f(1, 1, 0);
	for (float i = M_PI; i > max; i -= (M_PI / 24) ) {
		if (i < 5 * M_PI / 8)
		glColor3f(1, 0, 0);
		glVertex2f(bigWidth * cos(i) + 1, bigRadius * sin(i) - 1);
		glVertex2f(bigWidth * cos(i - (M_PI / 32)) + 1, bigRadius * sin(i - (M_PI / 32)) - 1);
		glVertex2f(smallWidth * cos(i - (M_PI / 32)) + 1, smallRadius * sin(i - (M_PI / 32)) - 1);
		glVertex2f(smallWidth * cos(i) + 1, smallRadius * sin(i) - 1);
	}
	glEnd();
	*/


	//Energy bar
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




	//Timer
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_BIAS, 0);
	glPixelTransferf(GL_BLUE_BIAS, 0);

	int minutes = (int)roundTimer / 60;
	int seconds = (int)roundTimer % 60;

	char timerStr[100];
	if (seconds < 10) {
		sprintf(timerStr, "0%d:0%d", minutes, seconds);
	}
	else {
		sprintf(timerStr, "0%d:%d", minutes, seconds);
	}
	font.FaceSize(scale * defaultFontSize / 2);
	font.Render(timerStr, -1, FTPoint(scale * 50, (height - scale * 50), 0));
	glPopAttrib();

	//a quad to be textured
	float timerHeight = 0.15;
	float timerWidth = (height * timerHeight) / width;
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex2f(-1, 1);
	glVertex2f(-1 + timerWidth, 1);
	glVertex2f(-1 + timerWidth, 1 - timerHeight);
	glVertex2f(-1, 1- timerHeight);
	glDisable(GL_TEXTURE_2D);
	glEnd();
}

void HUD::updateTime(float time) {
	roundTimer = time;
}


}	// namespace ui
}	// namespace hyperbright