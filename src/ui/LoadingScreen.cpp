#include "LoadingScreen.h"


namespace hyperbright {
namespace ui {

LoadingScreen::LoadingScreen() :
	state(State::LOADING1),
	loadingStart(std::make_shared<openGLHelper::Texture>("rsc/images/firstLoading1.png")),
	loadingMid(std::make_shared<openGLHelper::Texture>("rsc/images/firstLoading2.png")),
	loadingDone1(std::make_shared<openGLHelper::Texture>("rsc/images/firstLoading3.png")),
	loadingDone2(std::make_shared<openGLHelper::Texture>("rsc/images/secondLoading3.png")),
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

LoadingScreen::State LoadingScreen::getState() const { return state; }

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
	case hyperbright::ui::LoadingScreen::State::WAITING1:
		quad->setTexture(loadingDone1);
		break;
	case hyperbright::ui::LoadingScreen::State::WAITING2:
		quad->setTexture(loadingDone2);
		break;
	case hyperbright::ui::LoadingScreen::State::DONE:
	default:
		break;
	}
}

}	// namespace ui
}	// namespace hyperbright