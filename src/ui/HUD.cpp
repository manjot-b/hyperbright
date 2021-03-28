#define _USE_MATH_DEFINES
#include "HUD.h"

#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include "render/Renderer.h"

namespace hyperbright {
namespace ui {

HUD::HUD(float s, float e, const entity::Arena& arena) :
	font("rsc/fonts/neon_pixel-7.ttf"), defaultFontSize(150.f), speed(s), energy(e), miniMapPos(0.f, 50.f, 0.1f)
{
	miniMapTexture = std::make_shared<openGLHelper::Texture>(256, 256, false);
	miniMapBuffer = std::make_unique<openGLHelper::FrameBuffer>(miniMapTexture, false);

	timerTexture = std::make_shared<openGLHelper::Texture>("rsc/images/timer2.png");
	
	quadShader = std::make_shared<openGLHelper::Shader>("rsc/shaders/quad_vertex.glsl", "rsc/shaders/quad_fragment.glsl");
	quadShader->link();
	quad = std::make_unique<openGLHelper::Quad>(quadShader, miniMapTexture);
	//quad = std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/tree.jpeg"));

	float orthoSize = arena.getArenaSize().x * .5f * arena.getTileWidth();
	miniMapOrtho = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, .1f, 300.f);
	miniMapView = glm::lookAt(
		miniMapPos,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f)
	);
}

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

	// Render minimap
	// For some reason this needs to happen before rendering the quads for the energy bar
	// down below. Otherwise those quads don't render.
	quad->getShader()->use();
	quad->normalizeToViewport(width, height);
	quad->translate(glm::vec2(.8f, .3f));
	quad->scale(.35f);
	quad->render();
	glUseProgram(0);

	//Timer
	timerQuad = std::make_unique<openGLHelper::Quad>(quadShader, timerTexture);
	timerQuad->getShader()->use();
	timerQuad->normalizeToViewport(width, height);
	timerQuad->translate(glm::vec2(-0.96f, 0.52f));
	timerQuad->scale(.08f);
	timerQuad->render();
	glUseProgram(0);
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

	//a quad to be textured
	/*float timerHeight = 0.15;
	float timerWidth = (height * timerHeight) / width;
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex2f(-1, 1);
	glVertex2f(-1 + timerWidth, 1);
	glVertex2f(-1 + timerWidth, 1 - timerHeight);
	glVertex2f(-1, 1 - timerHeight);
	glEnd();*/

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
	glPopAttrib();

	
}

void HUD::preRenderMiniMap()
{
	glViewport(0, 0, miniMapTexture->getWidth(), miniMapTexture->getHeight());
	miniMapBuffer->bind();
	glClearColor(0.05f, 0.05f, 0.23f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//update roundTimer
void HUD::updateTime(float time) {
	roundTimer = time;
}

//update vehicle speed and energy
void HUD::update(float s, float e) {
	speed = s;
	energy = e;
}

const glm::vec3& HUD::getMiniMapCameraPos() { return miniMapPos; }
const glm::mat4& HUD::getMiniMapCameraView() { return miniMapView; }
const glm::mat4& HUD::getMiniMapOrtho() { return miniMapOrtho; }

}	// namespace ui
}	// namespace hyperbright