#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include "Renderer.hpp"


int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vulkan ImGui Example", nullptr, nullptr);
    if (!glfwVulkanSupported())
    {
        std::cout << "GLFW: Vulkan Not Supported" << std::endl;
        return 1;
    }



    // Initialize Vulkan

    // Create the ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Initialize the ImGui GLFW and Vulkan backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    /*TODO Initaalize ImGui InitInfo*/

    //ImGui_ImplVulkan_Init(&init_info, /* Your render pass */);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render your ImGui elements here

        ImGui::Render();

        // Submit ImGui draw data to Vulkan
        // You need to set up a command buffer for rendering ImGui elements

        // Submit your own application's rendering command buffer to Vulkan

        // Present the frame
    }

    // Cleanup

    // Destroy ImGui Vulkan and GLFW backends
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    // Destroy the ImGui context
    ImGui::DestroyContext();

    // Destroy Vulkan resources

    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}