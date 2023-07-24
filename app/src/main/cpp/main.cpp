#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include "Renderer.hpp"


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

std::vector<const char*> getValidationLayers()
{
    std::vector<const char*> validationLayers;

    if(enableValidationLayers)
    {
        validationLayers.push_back( "VK_LAYER_KHRONOS_validation");
    }

    if (!checkValidationLayerSupport(validationLayers))
    {
        throw std::runtime_error("validation layer requested, but not available!");
    }

    return validationLayers;
}

bool checkValidationLayerSupport(const std::vector<const char*> validationLayers)
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (const auto* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

std::vector<const char*> getEnabledExtensions()
{
    std::vector<const char*> extensions;

    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);


    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

vk::Instance createInstance()
{
    vk::ApplicationInfo appInfo { "Android Vulkan Demo",
                                  1,
                                  "No Engine",
                                  1,
                                  VK_API_VERSION_1_2 };


    auto enabledLayers = getValidationLayers();
    auto enabledExtensions = getEnabledExtensions();


    vk::InstanceCreateInfo instanceCreateInfo { {}, &appInfo, enabledLayers, enabledExtensions };


    return vk::createInstance(instanceCreateInfo);
}


vk::SurfaceKHR createSurface(vk::Instance& instance, GLFWwindow* window)
{
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    return  static_cast<vk::SurfaceKHR>(surface);
}

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

    vk::Instance instance = createInstance();
    vk::SurfaceKHR surface = createSurface(instance, window);

    VulkanRendererValidationLayerLevel validationLayer = enableValidationLayers ? 
                                                            VulkanRendererValidationLayerLevel::eEnabled :
                                                            VulkanRendererValidationLayerLevel::eNone;

    // Initialize Vulkan
    VulkanRenderer vulkanRenderer;
    vulkanRenderer.initRenderer(instance,
                                surface,
                                VulkanRendererValidationLayerLevel::eEnabled);


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