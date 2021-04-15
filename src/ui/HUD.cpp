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

HUD::HUD(std::shared_ptr<entity::Vehicle> v, const entity::Arena& arena, float& roundTimer) :
	font("rsc/fonts/neon_pixel-7.ttf"), defaultFontSize(150.f),
	player(v), roundTimer(roundTimer),
	miniMapPos(0.f, 50.f, 0.1f), 
	state(State::INTRO)
{
	updateWindowAndFontSize();

	miniMapTexture = std::make_shared<openGLHelper::Texture>(256, 256, false);
	miniMapBuffer = std::make_unique<openGLHelper::FrameBuffer>(miniMapTexture, false);

	timerTexture = std::make_shared<openGLHelper::Texture>("rsc/images/timer2.png");
	
	quadShader = std::make_shared<openGLHelper::Shader>("rsc/shaders/quad_vertex.glsl", "rsc/shaders/quad_fragment.glsl");
	quadShader->link();
	
	miniMap = std::make_unique<openGLHelper::Quad>(quadShader, miniMapTexture);
	
	glm::vec2 scale = glm::vec2(1.f, (float)width / height);
	scale *= .35f;
	miniMap->scale(scale);
	miniMap->translate( glm::vec2(1.f - miniMap->getWidth() * .5f, 1.f - miniMap->getHeight() * .5f) / scale);
	
	timerQuad = std::make_unique<openGLHelper::Quad>(quadShader, timerTexture);
	emp = std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/emp.png"));
	zap = std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/zap.png"));
	speedz = std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/speed.png"));
	slowtrap = std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/slowtrap.png"));
	syphon = std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/syphon.png"));
	slowed = std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/slowactive.png"));
	zapped = std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/zapactive.png"));
	syphonOn = std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/syphonactive.png"));

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

void HUD::displayCountdown() {
	if (countdownTimer <= 0.1f) {
		startCountdown = false;
		return;
	}

	updateWindowAndFontSize();
	float scale = (width * 0.125) / defaultFontSize;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_BIAS, -0.7);
	glPixelTransferf(GL_BLUE_BIAS, -0.7);
	int intCountdown = 1 + countdownTimer;
	float countdownScale = 1 - (intCountdown - countdownTimer);
	float xCord = (float)width - (3 * 63 * scale);
	float yCord = 45 * scale;

	char speedStr[100];
	sprintf(speedStr, "%i", intCountdown);
	font.FaceSize(scale * defaultFontSize * 2.f * countdownScale);
	font.Render(speedStr, -1, FTPoint(width / 2.f, height / 2.f + 100 * scale, 0));
}

void HUD::drawHUD() {
	updateWindowAndFontSize();
	float scale = (width * 0.125) / defaultFontSize;

	if (startCountdown) displayCountdown();

	if (state == State::PLAY) {
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


		//pickups
		if (player->hasPickup()) {
			pickup = player->getPickup();
			switch (pickup->type) {
			case (0):
				emp->getShader()->use();
				emp->normalizeToViewport(width, height);
				emp->translate(glm::vec2(-0.9f, 0.3f));
				emp->scale(.12f);
				emp->render();
				glUseProgram(0);
				break;
			case(1):
				zap->getShader()->use();
				zap->normalizeToViewport(width, height);
				zap->translate(glm::vec2(-0.9f, 0.3f));
				zap->scale(.2f);
				zap->render();
				glUseProgram(0);
				break;
			case(2):
				speedz->getShader()->use();
				speedz->normalizeToViewport(width, height);
				speedz->translate(glm::vec2(-0.9f, 0.3f));
				speedz->scale(.2f);
				speedz->render();
				glUseProgram(0);
				break;
			case(3):
				slowtrap->getShader()->use();
				slowtrap->normalizeToViewport(width, height);
				slowtrap->translate(glm::vec2(-0.9f, 0.3f));
				slowtrap->scale(.2f);
				slowtrap->render();
				glUseProgram(0);
				break;
			case(4):
				syphon->getShader()->use();
				syphon->normalizeToViewport(width, height);
				syphon->translate(glm::vec2(-0.9f, 0.3f));
				syphon->scale(.2f);
				syphon->render();
				glUseProgram(0);
				break;
			}
		}


		//Displaying a message when player is affected by a powerup
		if (player->syphonActive == true) {
			syphonOn->getShader()->use();
			syphonOn->normalizeToViewport(width, height);
			syphonOn->translate(glm::vec2(0.f, 0.2f));
			syphonOn->scale(1.f);
			syphonOn->render();
			glUseProgram(0);
		}
		else if (player->zapActive == true) {
			zapped->getShader()->use();
			zapped->normalizeToViewport(width, height);
			zapped->translate(glm::vec2(0.f, 0.1f));
			zapped->scale(1.f);
			zapped->render();
			glUseProgram(0);
		}
		else if (player->getController().trap.second == true) {
			slowed->getShader()->use();
			slowed->normalizeToViewport(width, height);
			slowed->translate(glm::vec2(0.f, 0.1f));
			slowed->scale(1.f);
			slowed->render();
			glUseProgram(0);
		}



		//Speedometer
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPixelTransferf(GL_RED_BIAS, 0);
		glPixelTransferf(GL_GREEN_BIAS, -0.7);
		glPixelTransferf(GL_BLUE_BIAS, -1);
		float xCord = (float)width - (3 * 63 * scale);
		float yCord = 45 * scale;
		double realSpeed = std::max(round(abs(player->readSpeedometer()) * 3), 0.f);

		char speedStr[100];
		sprintf(speedStr, "%d", (int)realSpeed);
		font.FaceSize(scale * defaultFontSize);
		font.Render(speedStr, -1, FTPoint(xCord, yCord, 0));
		font.FaceSize(scale * defaultFontSize / 3);
		font.Render("KPH", -1, FTPoint(width - (3 * 21 * scale), 10 * scale, 0));

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
		int numOfBar = (int)(player->energy * 25);
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
}

void HUD::preRenderMiniMap()
{
	glViewport(0, 0, miniMapTexture->getWidth(), miniMapTexture->getHeight());
	miniMapBuffer->bind();
	glClearColor(0.05f, 0.05f, 0.23f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//update roundTimer
// *NOT BEING USED CURRENTLY*
void HUD::updateTime(float time) {
	roundTimer = time;
}

//update vehicle speed and energy
// *NOT BEING USED CURRENTLY*
void HUD::update(float s, float e) {
	speed = s;
	energy = e;
}

const glm::vec3& HUD::getMiniMapCameraPos() { return miniMapPos; }
const glm::mat4& HUD::getMiniMapCameraView() { return miniMapView; }
const glm::mat4& HUD::getMiniMapOrtho() { return miniMapOrtho; }

}	// namespace ui
}	// namespace hyperbright