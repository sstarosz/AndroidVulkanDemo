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

static constexpr uint32_t initialWindowsWidth = 1280;
static constexpr uint32_t initialWindowsHeight = 720;

static void check_vk_result(VkResult err)
{
    if (err == 0)
    {
        return;
    }
    std::cerr << "[vulkan] Error: VkResult =" << err << std::endl;
    if (err < 0)
    {
        abort();
    }
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

std::vector<const char*> getEnabledExtensions()
{
    std::vector<const char*> extensions;

    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    std::vector<const char*> glfwExtensions;
    uint32_t extensions_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    for (uint32_t i = 0; i < extensions_count; i++)
    {
        glfwExtensions.push_back(glfw_extensions[i]);
    }

    for(const auto& extension : glfwExtensions)
    {
        extensions.push_back(extension);
    }


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


    vk::InstanceCreateInfo instanceCreateInfo { 
        {},
        &appInfo,
        enabledLayers,
        enabledExtensions };


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
    GLFWwindow* window = glfwCreateWindow(initialWindowsWidth,
                                            initialWindowsHeight,
                                            "Vulkan ImGui Example",
                                            nullptr,
                                            nullptr);
    if (!glfwVulkanSupported())
    {
        std::cout << "GLFW: Vulkan Not Supported" << std::endl;
        return 1;
    }

    //Initialize Renderer
    vk::Instance instance = createInstance();
    vk::SurfaceKHR surface = createSurface(instance, window);

    VulkanRendererValidationLayerLevel validationLayer = enableValidationLayers ? 
                                                            VulkanRendererValidationLayerLevel::eEnabled :
                                                            VulkanRendererValidationLayerLevel::eNone;


    VulkanRenderer vulkanRenderer;
    vulkanRenderer.setupSwapchain(initialWindowsWidth, initialWindowsHeight);
    vulkanRenderer.initRenderer(instance,
                                surface,
                                VulkanRendererValidationLayerLevel::eEnabled);


    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    // Initialize the ImGui GLFW and Vulkan backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vulkanRenderer.getInstance();
    init_info.PhysicalDevice = vulkanRenderer.getPhysicalDevice();
    init_info.Device = vulkanRenderer.getLogicalDevice();
    init_info.QueueFamily = vulkanRenderer.getQueueFamilyIndex();
    init_info.Queue = vulkanRenderer.getQueue();
    init_info.PipelineCache = {};
    init_info.DescriptorPool = vulkanRenderer.getUiDescriptorPool();
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 2;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = {};
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, vulkanRenderer.getUiRenderPass());



    /*{
        // Use any command queue
        VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
        VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

        err = vkResetCommandPool(g_Device, command_pool, 0);
        check_vk_result(err);
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(command_buffer, &begin_info);
        check_vk_result(err);

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        err = vkEndCommandBuffer(command_buffer);
        check_vk_result(err);
        err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
        check_vk_result(err);

        err = vkDeviceWaitIdle(g_Device);
        check_vk_result(err);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }*/


    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render your ImGui elements here
        
        ImGui::ShowDemoWindow();


        ImGui::Render();
        //renderer->update
        vulkanRenderer.renderFrame();

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