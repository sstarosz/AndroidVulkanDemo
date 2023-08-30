#ifdef Renderer_EXPORTS
#define Renderer_API __declspec(dllexport)
#else
#define Renderer_API __declspec(dllimport)
#endif


#include <vulkan/vulkan.hpp>
#include <vector>
#include <optional>
#include <array>
#include <ostream>

enum class VulkanRendererValidationLayerLevel
{
    eNone,
    eEnabled
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const;


    static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
};

struct SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;

    static SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
};






class VulkanRenderer
{

public:
    Renderer_API void initRenderer(vk::Instance& instance,
                                   vk::SurfaceKHR& surface,
                                   VulkanRendererValidationLayerLevel debugLevel);

    Renderer_API void setupSwapchain(uint32_t width, uint32_t height);


    Renderer_API void resizeFramebuffer(uint32_t width, uint32_t height);

    Renderer_API vk::Instance getInstance() const;
    Renderer_API vk::PhysicalDevice getPhysicalDevice() const;
    Renderer_API vk::Device getLogicalDevice() const;
    Renderer_API uint32_t getQueueFamilyIndex() const;
    Renderer_API vk::Queue getQueue() const;
    Renderer_API vk::DescriptorPool getUiDescriptorPool() const;
    Renderer_API vk::RenderPass getUiRenderPass() const;

private:
    void initVulkan();
    void cleanup();

    void createDebugMessenger();
    void pickPhysicalDevice();
    bool isDeviceSuitable(const vk::PhysicalDevice& device);
    bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);

    void createLogicalDevice();

    void createSwapChain();
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
	void createSwapchainImageViews();

    void createRenderPass();
    vk::Format findDepthFormat() const;
    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;

    void createGraphicsPipeline();
    void createTextureSampler();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSetLayout();


    void createUiGraphicsPipeline();



    void createBuffer(vk::DeviceSize size,
                      vk::BufferUsageFlags usage,
                      vk::MemoryPropertyFlags properties,
                      vk::Buffer& buffer,
                      vk::DeviceMemory& bufferMemory) const;
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

    VulkanRendererValidationLayerLevel m_enableValidationLayers;

    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;
    vk::SurfaceKHR  m_surface;
    vk::PhysicalDevice m_physicalDevice;
    
    vk::Device m_device;
    vk::Queue m_graphicsQueue;
	vk::Queue m_presentQueue;

    vk::SwapchainKHR m_swapChain;
    std::vector<vk::Image> m_swapChainImages;
    vk::Format m_swapChainImageFormat;
    vk::Extent2D m_swapChainExtent;
    std::vector<vk::ImageView> m_swapChainImageViews;
    uint32_t m_swapchainWidth;
    uint32_t m_swapchainHeight;

    vk::RenderPass m_renderPass;
    vk::ImageView m_depthImageView;

    vk::Pipeline m_graphicsPipeline;
    vk::PipelineLayout m_pipelineLayout;
    vk::PipelineCache m_pipelineCache;
    std::vector<vk::DynamicState> m_dynamicStateEnables;
    vk::PipelineDynamicStateCreateInfo m_pipelineDynamicStateCreateInfo;

    //GraphicsPipeline
    vk::Sampler m_textureSampler;
    std::vector<vk::Buffer> m_uniformBuffers;
	std::vector<vk::DeviceMemory> m_uniformBuffersMemory;
    vk::DescriptorPool m_primitiveDescriptorPool;
    vk::DescriptorSetLayout m_descriptorSetLayout;

    constexpr static std::array m_deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    constexpr static uint32_t MAX_FRAMES_IN_FLIGHT{2};

};



#ifdef __ANDROID__
extern "C"
{


    JNIEXPORT jstring JNICALL
    Java_com_st_androidvulkandemo_MainActivity_stringFromJNI(JNIEnv *env,   jobject /* this */)
    {
        std::string hello = "Hello from C++";
        return env->NewStringUTF(hello.c_str());
    }


    JNIEXPORT jstring JNICALL
    Java_com_st_androidvulkandemo_MainActivity_InitVulkanRenderer(JNIEnv *env,   jobject /* this */)
    {
        std::string hello = "Hello from C++";
        return env->NewStringUTF(hello.c_str());
    }

}
#endif