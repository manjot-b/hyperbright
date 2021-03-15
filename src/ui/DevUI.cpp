#include "DevUI.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "engine/TeamStats.h"

namespace hyperbright {
namespace ui {

DevUI::DevUI(GLFWwindow* window) : showDemo(false)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

DevUI::~DevUI() {}

void DevUI::update(float deltaSec, float roundTimer)
{
    _deltaSec = deltaSec;
    _roundTimer = roundTimer;
}

/**
 * This method should be called every frame by the Renderer. 
*/
void DevUI::render()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showDemo)
    {
        ImGui::ShowDemoWindow(&showDemo);
    }

    ImGui::Begin("Dev Settings");
    ImGui::Text("Application average %.3f ms/frame %.3f FPS", _deltaSec*1000, 1/_deltaSec);
    ImGui::Text("Time: %.2f s", _roundTimer);
    ImGui::SliderInt("FPS Cap", &(settings.fps), 30, 144);
    ImGui::Text("Scores:");
    for (unsigned int i = static_cast<unsigned int>(engine::teamStats::Teams::TEAM0); i != static_cast<unsigned int>(engine::teamStats::Teams::LAST); i++)
    {
        engine::teamStats::Teams team = static_cast<engine::teamStats::Teams>(i);
        ImGui::Text("Team %u: %u", i, engine::teamStats::scores[team]);
    }

    // Vehicle Body Material settings
    ImGui::Text("Body material:");
    ImGui::SliderInt("Shading Model", reinterpret_cast<int*>(&(settings.vehicleBodyMaterial.shadingModel)), 0, 1, "Phong or Cook-Torrance");
    ImGui::SliderFloat("Diffuse Strength", &settings.vehicleBodyMaterial.diffuse, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular Strength", &settings.vehicleBodyMaterial.specular, 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &settings.vehicleBodyMaterial.shininess, 0.01f, 1000.0f);
    ImGui::SliderFloat("Roughness", &settings.vehicleBodyMaterial.roughness, 0.01f, 1.0f);
    ImGui::ColorEdit3("Fresnel", (float*)&settings.vehicleBodyMaterial.fresnel);
    ImGui::Checkbox("Beckmann NDF", &settings.vehicleBodyMaterial.useBeckmann);
    ImGui::Checkbox("GGX NDF", &settings.vehicleBodyMaterial.useGGX);
    

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}	// namespace ui
}	// namespace hyperbright