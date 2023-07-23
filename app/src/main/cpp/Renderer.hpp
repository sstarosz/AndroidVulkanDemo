#ifdef Renderer_EXPORTS
#define Renderer_API __declspec(dllexport)
#else
#define Renderer_API __declspec(dllimport)
#endif


#include <vulkan/vulkan.hpp>



class VulkanRenderer
{

public:
    Renderer_API void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();


    void createInstance();
    void createDebugMessenger();
    void createSurface();

    vk::Instance mInstance;
    vk::DebugUtilsMessengerEXT mDebugMessenger;
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