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
		DONE
	};
	LoadingScreen();
	void render() const;

private:
	State state;
	std::shared_ptr<openGLHelper::Texture> loadingStart;
	std::shared_ptr<openGLHelper::Texture> loadingMid;
	std::shared_ptr<openGLHelper::Texture> loadingDone;
	
	std::shared_ptr<openGLHelper::Shader> quadShader;
	std::unique_ptr<openGLHelper::Quad> quad;
	
	State getState();
	void setState(State state);

};

}	// namespace ui
}	// namespace hyperbright