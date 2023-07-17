#include <jni.h>
#include <android/log.h>
#include <string>
#include <sstream>

#define VK_USE_PLATFORM_ANDROID_KHR 0
#include <vulkan/vulkan.hpp>


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

[[maybe_unused]] VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                                               const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                               const VkAllocationCallbacks* pAllocator,
                                                                               VkDebugUtilsMessengerEXT* pMessenger)
{
    return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

[[maybe_unused]] VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                                            VkDebugUtilsMessengerEXT messenger,
                                                                            VkAllocationCallbacks const* pAllocator)
{
    return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}


VKAPI_ATTR  VkBool32  VKAPI_CALL  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                       void* pUserData)
{
    std::ostringstream message;

    message << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << ": "
            << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageType)) << ":\n";
    message << "\t"
            << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
    message << "\t"
            << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
    message << "\t"
            << "message         = <" << pCallbackData->pMessage << ">\n";

    if (0 < pCallbackData->queueLabelCount)
    {
        message << "\t"
                << "Queue Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
        {
            message << "\t\t"
                    << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
        }
    }

    if (0 < pCallbackData->cmdBufLabelCount)
    {
        message << "\t"
                << "CommandBuffer Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
        {
            message << "\t\t"
                    << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
        }
    }

    if (0 < pCallbackData->objectCount)
    {
        message << "\t"
                << "Objects:\n";
        for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
        {
            message << "\t\t"
                    << "Object " << i << "\n";
            message << "\t\t\t"
                    << "objectType   = " << vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) << "\n";
            message << "\t\t\t"
                    << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
            if (pCallbackData->pObjects[i].pObjectName)
            {
                message << "\t\t\t"
                        << "objectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
            }
        }
    }

    __android_log_print(ANDROID_LOG_DEBUG, "Vulkan Renderer", "Debug message: %s", message.str().c_str());

    return false;
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
    std::vector<const char*> validationLayers { "VK_LAYER_KHRONOS_validation" };

    if (!checkValidationLayerSupport(validationLayers))
    {
        //TODO - Better validation layer handling
        throw std::runtime_error("validation layer requested, but not available!");
    }

    return validationLayers;
}

std::vector<const char*> getEnabledExtensions()
{
    std::vector<const char*> extensions;

    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

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
    void createDebugMessenger();
    void createSurface();

    vk::Instance mInstance;
    vk::DebugUtilsMessengerEXT mDebugMessenger;
};

void VulkanRenderer::initVulkan()
{
    createInstance();
}

void VulkanRenderer::createInstance()
{
    vk::ApplicationInfo appInfo { "Cloth Simulation App",
                                  1,
                                  "No Engine",
                                  1,
                                  VK_API_VERSION_1_2 };


    auto validationLayers = getValidationLayers();
    auto enabledExtensions = getEnabledExtensions();


    vk::InstanceCreateInfo instanceCreateInfo { {}, &appInfo, validationLayers, enabledExtensions };


    mInstance = vk::createInstance(instanceCreateInfo);

}

void VulkanRenderer::createDebugMessenger()
{
    pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(mInstance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!pfnVkCreateDebugUtilsMessengerEXT)
    {
        //TODO - Change it to something independent of iostream
        __android_log_print(ANDROID_LOG_DEBUG, "Vulkan Renderer", "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");
        exit(1);
    }

    pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(mInstance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!pfnVkDestroyDebugUtilsMessengerEXT)
    {
        //TODO - Change it to something independent of iostream
        __android_log_print(ANDROID_LOG_DEBUG, "Vulkan Renderer", "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.");
        exit(1);
    }


    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags { vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                          vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                          vk::DebugUtilsMessageSeverityFlagBitsEXT::eError };

    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags { vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                         vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                         vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation };

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT { {}, severityFlags, messageTypeFlags, &debugCallback };


    mDebugMessenger = mInstance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

void VulkanRenderer::createSurface()
{

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