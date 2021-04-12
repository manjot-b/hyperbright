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

    // set default settings
        // volume
    settings.musicVolume = 0.f;

        // camera
    setCameraDefaults();

        // handling
    setHandlingDefaults();

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

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::Begin("Dev Settings", NULL, window_flags);
    ImGui::Text("Application average %.3f ms/frame %.3f FPS", _deltaSec*1000, 1/_deltaSec);
    ImGui::Text("Time: %.2f s", _roundTimer);
    ImGui::SliderInt("FPS Cap", &(settings.fps), 30, 144);
    ImGui::SliderFloat("Music Volume", &settings.musicVolume, 0.f, 1.f);
    ImGui::Text("Scores:");
    for (unsigned int i = static_cast<unsigned int>(engine::teamStats::Teams::TEAM0); i != static_cast<unsigned int>(engine::teamStats::Teams::LAST); i++)
    {
        engine::teamStats::Teams team = static_cast<engine::teamStats::Teams>(i);
        ImGui::Text("Team %u: %u  ", i, engine::teamStats::scores[team]);
        ImGui::SameLine();
    }
    ImGui::Spacing();

    // Vehicle Body Material settings
    ImGui::BeginChild("Vehicle Body Material Settings", ImVec2(500, 150), true);
    ImGui::Text("Body material:");
    ImGui::SliderInt("Shading Model", reinterpret_cast<int*>(&(settings.vehicleBodyMaterial.shadingModel)), 0, 1, "Phong or Cook-Torrance");
    ImGui::SliderFloat("Diffuse Strength", &settings.vehicleBodyMaterial.diffuse, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular Strength", &settings.vehicleBodyMaterial.specular, 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &settings.vehicleBodyMaterial.shininess, 0.01f, 1000.0f);
    ImGui::SliderFloat("Roughness", &settings.vehicleBodyMaterial.roughness, 0.01f, 1.0f);
    ImGui::ColorEdit3("Fresnel", (float*)&settings.vehicleBodyMaterial.fresnel);
    ImGui::Checkbox("Beckmann NDF", &settings.vehicleBodyMaterial.useBeckmann);
    ImGui::Checkbox("GGX NDF", &settings.vehicleBodyMaterial.useGGX);
    ImGui::EndChild();

    // Camera Settings
    ImGui::Text("Camera Settings:");
    ImGui::BeginChild("Camera Settings", ImVec2(500, 180), true);
    ImGui::SliderFloat("Camera Height", &settings.cameraHeight, 1.f, 7.f);
    ImGui::SliderFloat("Camera Velocity Coef", &settings.cameraVelocityCoeficient, 0.f, 10.f);
    ImGui::SliderFloat("Camera Rest Length", &settings.cameraRestLength, 1.f, 20.f);
    ImGui::SliderFloat("Camera Swing Strength", &settings.cameraSwingStrength, 0.001f, 0.1f);
    ImGui::SliderFloat("POI Height", &settings.poiHeight, -4.f, 4.f);
    ImGui::SliderFloat("POI Depth", &settings.poiDepth, 0.f, 20.f);
    if (ImGui::Button("Reset")) setCameraDefaults();
    ImGui::EndChild();

    // Handling Settings
    ImGui::Text("Vehicle Handling:");
    ImGui::BeginChild("Vehicle Handling", ImVec2(500, 220), true);
    ImGui::SliderFloat("Velocity Interval 1", &settings.handling.velStepOne,                          0.f, settings.handling.velStepTwo);
    ImGui::SliderFloat("Turning at Vel 1", &settings.handling.stepOneTurnStr, 0.f, 1.f);
    ImGui::SliderFloat("Velocity Interval 2", &settings.handling.velStepTwo, settings.handling.velStepOne, settings.handling.velStepThr);
    ImGui::SliderFloat("Turning at Vel 2", &settings.handling.stepTwoTurnStr, 0.f, 1.f);
    ImGui::SliderFloat("Velocity Interval 3", &settings.handling.velStepThr, settings.handling.velStepTwo, settings.handling.velStepFou);
    ImGui::SliderFloat("Turning at Vel 3", &settings.handling.stepThrTurnStr, 0.f, 1.f);
    ImGui::SliderFloat("Velocity Interval 4", &settings.handling.velStepFou, settings.handling.velStepThr,                        100.f);
    ImGui::SliderFloat("Turning at Vel 4", &settings.handling.stepFouTurnStr, 0.f, 1.f);
    if (ImGui::Button("Reset")) setHandlingDefaults();
    ImGui::EndChild();
    // Player Vehicle Driving Settings
    ImGui::End();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DevUI::setCameraDefaults() {
    settings.cameraHeight = 2.5f;
    settings.cameraVelocityCoeficient = 4.f;
    settings.cameraRestLength = 7.5f;
    settings.cameraSwingStrength = 0.07f;
    settings.poiHeight = -0.4f;
    settings.poiDepth = 2.f;
}

void DevUI::setHandlingDefaults()
{
    settings.handling.velStepOne = 0.f;
    settings.handling.velStepTwo = 10.f;
    settings.handling.velStepThr = 20.f;
    settings.handling.velStepFou = 60.f;
    settings.handling.stepOneTurnStr = 0.8f;
    settings.handling.stepTwoTurnStr = 0.4f;
    settings.handling.stepThrTurnStr = 0.3f;
    settings.handling.stepFouTurnStr = 0.2f;
}

}	// namespace ui
}	// namespace hyperbright