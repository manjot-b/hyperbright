#include "LoadingScreen.h"


namespace hyperbright {
namespace ui {

LoadingScreen::LoadingScreen() :
	state(State::LOADING1),
	loadingStart(std::make_shared<openGLHelper::Texture>("rsc/images/firstLoading1.png")),
	loadingMid(std::make_shared<openGLHelper::Texture>("rsc/images/secondLoading2.png")),
	loadingDone(std::make_shared<openGLHelper::Texture>("rsc/images/secondLoading3.png")),
	quadShader(std::make_shared<openGLHelper::Shader>("rsc/shaders/quad_vertex.glsl", "rsc/shaders/quad_fragment.glsl")),
	quad(std::make_unique<openGLHelper::Quad>(quadShader, loadingStart))
{
	
	quadShader->link();
	quad->scale(2.f);
}

void LoadingScreen::render() const
{
	quad->getShader()->use();
	quad->render();
	glUseProgram(0);
}

LoadingScreen::State LoadingScreen::getState() { return state; }

void LoadingScreen::setState(State state)
{
	this->state = state;
	switch (state)
	{
	case hyperbright::ui::LoadingScreen::State::LOADING1:
		quad->setTexture(loadingStart);
		break;
	case hyperbright::ui::LoadingScreen::State::LOADING2:
		quad->setTexture(loadingMid);
		break;
	case hyperbright::ui::LoadingScreen::State::DONE:
		quad->setTexture(loadingDone);
		break;
	}
}

}	// namespace ui
}	// namespace hyperbright