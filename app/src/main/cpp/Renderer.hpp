#ifdef Renderer_EXPORTS
#define Renderer_API __declspec(dllexport)
#else
#define Renderer_API __declspec(dllimport)
#endif


#include <vulkan/vulkan.hpp>
#include <vector>

enum class VulkanRendererValidationLayerLevel
{
    eNone,
    eEnabled
};


class VulkanRenderer
{

public:
    Renderer_API void initRenderer(vk::Instance& instance,
                                   vk::SurfaceKHR& surface,
                                   VulkanRendererValidationLayerLevel debugLevel);

    


    Renderer_API void resizeFramebuffer(uint32_t width, uint32_t height);

private:
    void initVulkan();
    void cleanup();

    void createDebugMessenger();

    VulkanRendererValidationLayerLevel m_enableValidationLayers;

    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;
    vk::SurfaceKHR  m_surface;
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