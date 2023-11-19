#ifdef Renderer_EXPORTS
#define Renderer_API __declspec(dllexport)
#else
#define Renderer_API 
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



struct Texture;


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

    Renderer_API vk::CommandBuffer beginSingleTimeCommands();
    Renderer_API void endSingleTimeCommands(vk::CommandBuffer commandBuffer);


    Renderer_API void startFrame();
    Renderer_API vk::CommandBuffer beginUiRendering();
    Renderer_API void endUiRendering(vk::CommandBuffer& uiCommandBuffer );
    Renderer_API void endFrame();



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

    void updateGraphicPipelineRecourses();

    void createUiGraphicsPipeline();

    void createCommandPool();

    void createFramebuffer();
    void createDepthResources();

    void createVertexBuffer();
    void createIndexBuffer();
    void createCommandBuffers();
    void createSyncObjects();

    void updateUniformBuffer(uint32_t currentImage);
    void recordCommandBuffer(vk::CommandBuffer& commandBuffer, uint32_t imageIndex);

    void createBuffer(vk::DeviceSize size,
                      vk::BufferUsageFlags usage,
                      vk::MemoryPropertyFlags properties,
                      vk::Buffer& buffer,
                      vk::DeviceMemory& bufferMemory) const;
                      
    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

    void createImage(uint32_t width,
				 uint32_t height,
				 vk::Format format,
				 vk::ImageTiling tiling,
				 vk::ImageUsageFlags usage,
				 vk::MemoryPropertyFlags properties,
				 vk::Image& image,
				 vk::DeviceMemory& imageMemory) const;

    vk::ImageView createImageView(vk::Image image,
                                  vk::Format format,
                                  vk::ImageAspectFlags aspectFlags) const;

    void createTextureImage(Texture& texture, vk::Image& textureImage, vk::DeviceMemory& textureImageMemory);
    void createTextureImageView(vk::Image& textureImage, vk::ImageView& textureImageView);
    void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);


    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;


    void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

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
    std::vector<vk::DescriptorSet> m_descriptorSets;

    vk::Image m_textureImage;
    vk::DeviceMemory textureImageMemory;
    vk::ImageView m_textureImageView;



    vk::CommandPool m_commandPool;

    std::vector<vk::Framebuffer> m_swapchainFramebuffers;
    std::vector<vk::Framebuffer> m_uiSwapchainFramebuffers;

    vk::Image m_depthImage; 
    vk::DeviceMemory m_depthImageMemory;
    vk::ImageView m_depthImageView;

    constexpr static std::array m_deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    constexpr static uint32_t MAX_FRAMES_IN_FLIGHT{2};


    vk::Buffer m_planeVertexBuffer;
	vk::DeviceMemory m_planeVertexBufferMemory;
	vk::Buffer m_planeIndexBuffer;
	vk::DeviceMemory m_planeIndexBufferMemory;

    std::vector<vk::CommandBuffer> m_commandBuffers;
    std::vector<vk::CommandBuffer> m_uiCommandBuffers;


    std::vector<vk::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::Semaphore> m_uiAvailableSemaphores;
    std::vector<vk::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::Fence> m_inFlightFences;

    uint32_t currentFrame = 0;
    vk::Result currentFrameResult;
    uint32_t currentImageIndex = 0;


    bool m_framebufferResized = false;



    /*ImGui*/
    vk::DescriptorPool m_uiDescriptorPool;
    vk::RenderPass m_uiRenderPass;
    std::vector<vk::Image> m_uiSwapchainImages;
    std::vector<vk::ImageView> m_uiSwapchainImageViews;



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