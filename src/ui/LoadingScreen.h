#pragma once

#include <opengl-helper/Quad.h>
#include <opengl-helper/Texture.h>

#include <memory>

namespace hyperbright {
namespace ui {

class LoadingScreen
{
public:
	enum class State
	{
		LOADING1,
		LOADING2,
		WAITING,
		DONE
	};

	LoadingScreen();

	void render() const;
	State getState() const;
	void setState(State state);

private:
	State state;
	std::shared_ptr<openGLHelper::Texture> loadingStart;
	std::shared_ptr<openGLHelper::Texture> loadingMid;
	std::shared_ptr<openGLHelper::Texture> loadingDone;
	
	std::shared_ptr<openGLHelper::Shader> quadShader;
	std::unique_ptr<openGLHelper::Quad> quad;
};

}	// namespace ui
}	// namespace hyperbright