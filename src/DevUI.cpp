#include "DevUI.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

static int sliderFPS = 60;

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


/**
 * This method should be called every frame by the Renderer. 
*/
void DevUI::show(float deltaSec)
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
    ImGui::Text("Application average %.3f ms/frame %.3f FPS", deltaSec*1000, 1/deltaSec);
    ImGui::SliderInt("FPS Cap", &sliderFPS, 30, 144);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//return the current FPS Cap
int DevUI::getSliderFPS() {
    return sliderFPS;
}