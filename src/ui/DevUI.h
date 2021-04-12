#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "model/MeshUtils.h"

namespace hyperbright {
namespace ui {
class DevUI
{
public:
	struct Settings {
		int fps;
		model::Material vehicleBodyMaterial;

		float musicVolume;

		float cameraHeight;
		float cameraVelocityCoeficient;
		float cameraRestLength;
		float cameraSwingStrength;
		float poiHeight;
		float poiDepth;

		struct Handling {
			float velStepOne;
			float velStepTwo;
			float velStepThr;
			float velStepFou;
			float stepOneTurnStr;
			float stepTwoTurnStr;
			float stepThrTurnStr;
			float stepFouTurnStr;

			float analogAccel;
			float analogBrake;
			float analogHandBrake;
			float analogSteer;
									 
			float analogAccelFall;
			float analogBrakeFall;
			float analogHandBrakeFall;
			float analogSteerFall;
		} handling;
	} settings;	  

	DevUI(GLFWwindow* window);
	~DevUI();
	void update(float deltaSec, float roundTimer);
	void render();

	void setCameraDefaults();
	void setHandlingDefaults();
private:
	bool showDemo;
	float _deltaSec;
	float _roundTimer;
};
}	// namespace ui
}	// namespace hyperbright