#include "Renderer.hpp"

#ifdef __ANDROID__
    #include <jni.h>
    #include <android/log.h>
    #define VK_USE_PLATFORM_ANDROID_KHR 0
#endif


#include <string>
#include <sstream>
#include <iostream>

void printLog(const std::string& message)
{
    #ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_DEBUG, "Vulkan Renderer", "Debug message: %s", message.c_str());
    #else
        std::cout << "Debug message: " << message << std::endl;
    #endif
}


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

VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

        printLog(message.str());
        
		return false;
	}




void VulkanRenderer::initRenderer(vk::Instance& instance,
                                   vk::SurfaceKHR& surface,
                                   VulkanRendererValidationLayerLevel debugLevel)
{
    m_instance = instance;
    m_surface = surface;

    m_enableValidationLayers = debugLevel;

    //initWindow();
    initVulkan();
}






void VulkanRenderer::initVulkan()
{
    if(m_enableValidationLayers == VulkanRendererValidationLayerLevel::eEnabled)
    {
        createDebugMessenger();
    }
   //pickPhysicalDevice
}





void VulkanRenderer::cleanup()
{

}




void VulkanRenderer::createDebugMessenger()
{
    pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(m_instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!pfnVkCreateDebugUtilsMessengerEXT)
    {
        printLog("GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");
        exit(1);
    }

    pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(m_instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!pfnVkDestroyDebugUtilsMessengerEXT)
    {
        //TODO - Change it to something independent of iostream
        printLog("GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function."); 
        exit(1);
    }


    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags { vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                          vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                          vk::DebugUtilsMessageSeverityFlagBitsEXT::eError };

    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags { vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                         vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                         vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation };

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT { {}, severityFlags, messageTypeFlags, &debugCallback };


    m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}





