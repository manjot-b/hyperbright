#include "Engine.h"
#include "Renderer.h"

Engine::Engine() {
	
}

Engine::~Engine() {

}

void Engine::run() {
	Renderer renderer;
	renderer.run();
	return;
}