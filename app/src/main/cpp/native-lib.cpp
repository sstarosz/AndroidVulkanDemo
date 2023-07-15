#include <jni.h>
#include <string>

//#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.hpp>
//#include <vulkan/v

class VulkanRenderer
{

public:
    void run()
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

    vk::Instance mInstance;
};

void VulkanRenderer::initVulkan()
{
    createInstance();
}

void VulkanRenderer::createInstance()
{
    vk::ApplicationInfo appInfo { "Android Vulkan renderer",
                                  1,
                                  "No Engine",
                                  1,
                                  VK_API_VERSION_1_2 };

    vk::InstanceCreateInfo instanceCreateInfo{
            {}, &appInfo, {}, {}
    };

    mInstance = vk::createInstance(instanceCreateInfo);
}



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