#include "Renderer.hpp"

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>
#define VK_USE_PLATFORM_ANDROID_KHR 0
#else
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif




#include <string>
#include <sstream>
#include <iostream>
#include <set>
#include "Shaders/Shader.hpp"
#include "StMath/StMath.hpp"
#include "Camera.hpp"

struct UniformBufferObject
{
    st::math::Matrix4x4 model;
    st::math::Matrix4x4 view;
    st::math::Matrix4x4 proj;
};

struct Vertex
{
    st::math::Vector3 m_pos;
    st::math::Vector2 m_texCoord;
    st::math::Vector3 m_color;
    st::math::Vector3 m_normal;


    static vk::VertexInputBindingDescription getBindingDescription();

    static std::array<vk::VertexInputAttributeDescription, 4> getAttributeDescriptions();

    bool operator==(const Vertex&) const = default;
    auto operator<=>(const Vertex&) const = default;


    friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex)
    {
        os << "\nVertex(\n";
        os << "\tPos    {" << vertex.m_pos.X       << ", " << vertex.m_pos.Y      << ", " << vertex.m_pos.Z  << "}\n";
        os << "\tUV     {" << vertex.m_texCoord.X  << ", " << vertex.m_texCoord.Y << "}\n";
        os << "\tColor  {" << vertex.m_color.X     << ", " << vertex.m_color.Y    << ", " << vertex.m_color.Z  << "}\n";
        os << "\tNormal {" << vertex.m_normal.X    << ", " << vertex.m_normal.Y   << ", " << vertex.m_normal.Z << "}\n";
        os << ")\n";
        return os;
    }
};


struct Texture
{
	uint32_t textureWidth;
	uint32_t textureHeight;
	uint32_t texChannels;

	std::span<std::byte> pixels;
};

static const std::vector<Vertex> planeVertexes {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}, 
    {{ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}, 
    {{-0.5f, 0.5f,  0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}} 
};

static const std::vector<uint32_t> planeIndices = {
    0, 1, 2, 2, 3, 0
};

static st::renderer::Camera camera;



void printLog(const std::string &message)
{
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_DEBUG, "Vulkan Renderer", "Debug message: %s", message.c_str());
#else
	std::cout << "Debug message: " << message << std::endl;
#endif
}

bool QueueFamilyIndices::isComplete() const
{
	return graphicsFamily.has_value() && presentFamily.has_value();
}

QueueFamilyIndices QueueFamilyIndices::findQueueFamilies(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface)
{
	QueueFamilyIndices indices;

	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

	int i = 0;
	for (const auto &queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = device.getSurfaceSupportKHR(i, surface);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapChainSupportDetails SwapChainSupportDetails::querySwapChainSupport(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface)
{
	SwapChainSupportDetails details;

	details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
	details.formats = device.getSurfaceFormatsKHR(surface);
	details.presentModes = device.getSurfacePresentModesKHR(surface);

	return details;
}

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

[[maybe_unused]] VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
																			   const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
																			   const VkAllocationCallbacks *pAllocator,
																			   VkDebugUtilsMessengerEXT *pMessenger)
{
	return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

[[maybe_unused]] VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
																			VkDebugUtilsMessengerEXT messenger,
																			VkAllocationCallbacks const *pAllocator)
{
	return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
					   VkDebugUtilsMessageTypeFlagsEXT messageType,
					   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
					   void *pUserData)
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

/*
Public Api
*/

void VulkanRenderer::initRenderer(vk::Instance &instance,
                                  vk::SurfaceKHR &surface,
                                  VulkanRendererValidationLayerLevel debugLevel)
{
	m_instance = instance;
	m_surface = surface;

	m_enableValidationLayers = debugLevel;

	// initWindow();
	initVulkan();
}

void VulkanRenderer::setupSwapchain(uint32_t width, uint32_t height)
{
	m_swapchainWidth = width;
	m_swapchainHeight = height;
}

void VulkanRenderer::resizeFramebuffer(uint32_t width, uint32_t height)
{
}
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
/*--------------------------Getters-----------------------------------------------*/
/*--------------------------------------------------------------------------------*/

vk::Instance VulkanRenderer::getInstance() const
{
	return m_instance;
}

vk::PhysicalDevice VulkanRenderer::getPhysicalDevice() const
{
	return m_physicalDevice;
}

vk::Device VulkanRenderer::getLogicalDevice() const
{
	return m_device;
}

uint32_t VulkanRenderer::getQueueFamilyIndex() const
{
	return QueueFamilyIndices::findQueueFamilies(m_physicalDevice, m_surface).graphicsFamily.value();
}

vk::Queue VulkanRenderer::getQueue() const
{
	return m_graphicsQueue;
}

vk::DescriptorPool VulkanRenderer::getUiDescriptorPool() const
{
	return m_uiDescriptorPool;
}

vk::RenderPass VulkanRenderer::getUiRenderPass() const
{
	return m_uiRenderPass;
}


void VulkanRenderer::startFrame()
{
    auto resultFence = m_device.waitForFences(m_inFlightFences.at(currentFrame), VK_TRUE, UINT64_MAX);
	if (resultFence != vk::Result::eSuccess)
	{
		//std::cout << "syf" << std::endl;
	}

	auto [result, imageIndex] = m_device.acquireNextImageKHR(m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE);
	currentFrameResult = result;
	currentImageIndex = imageIndex;


	updateUniformBuffer(currentFrame);


	m_device.resetFences(m_inFlightFences.at(currentFrame));

	m_commandBuffers[currentFrame].reset(vk::CommandBufferResetFlags {});
	recordCommandBuffer(m_commandBuffers[currentFrame], currentImageIndex);


}


vk::CommandBuffer VulkanRenderer::beginUiRendering()
{
	
	m_uiCommandBuffers[currentFrame].begin(vk::CommandBufferBeginInfo {});

	//Draw primitive
	vk::Extent2D swapChainExtent = m_swapChainExtent;
	vk::RenderPassBeginInfo renderPassInfo { m_uiRenderPass,
											m_uiSwapchainFramebuffers[currentImageIndex],
											vk::Rect2D((0, 0), swapChainExtent)};

	m_uiCommandBuffers[currentFrame].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	return m_uiCommandBuffers[currentFrame];
}

void VulkanRenderer::endUiRendering(vk::CommandBuffer& uiCommandBuffer )
{

	uiCommandBuffer.endRenderPass();
	uiCommandBuffer.end();
}


void VulkanRenderer::endFrame()
{
	m_device.resetFences(m_inFlightFences.at(currentFrame));
	vk::PipelineStageFlags waitDestinationStageMask{vk::PipelineStageFlagBits::eColorAttachmentOutput};

	vk::SubmitInfo submitInfo(m_imageAvailableSemaphores[currentFrame],
								waitDestinationStageMask,
								m_commandBuffers[currentFrame],
								m_uiAvailableSemaphores[currentFrame]);

	m_graphicsQueue.submit(submitInfo, m_inFlightFences[currentFrame]);

	auto resultFence = m_device.waitForFences(m_inFlightFences.at(currentFrame), VK_TRUE, UINT64_MAX);
	m_device.resetFences(m_inFlightFences.at(currentFrame));
	vk::SubmitInfo uiSubmitInfo(m_uiAvailableSemaphores[currentFrame],
								waitDestinationStageMask,
								m_uiCommandBuffers[currentFrame],
								m_renderFinishedSemaphores[currentFrame]);

	m_graphicsQueue.submit(uiSubmitInfo, m_inFlightFences[currentFrame]);


	vk::PresentInfoKHR presentInfo{m_renderFinishedSemaphores[currentFrame], m_swapChain, currentImageIndex};

	try
	{
		currentFrameResult = m_presentQueue.presentKHR(presentInfo);
	}
	catch (std::exception const& exc)
	{
		std::cerr << exc.what();
		//TODO - Fix
		//recreateSwapChain();
	}

	if (currentFrameResult == vk::Result::eErrorOutOfDateKHR || currentFrameResult == vk::Result::eSuboptimalKHR || m_framebufferResized)
	{
		m_framebufferResized = false;
		//TODO - Fix
		//recreateSwapChain();
	}
	else if (currentFrameResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void VulkanRenderer::renderFrame(ImDrawData* imgui)
{
    auto resultFence = m_device.waitForFences(m_inFlightFences.at(currentFrame), VK_TRUE, UINT64_MAX);
	if (resultFence != vk::Result::eSuccess)
	{
		//std::cout << "syf" << std::endl;
	}

	auto [result, imageIndex] = m_device.acquireNextImageKHR(m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE);


	updateUniformBuffer(currentFrame);


	m_device.resetFences(m_inFlightFences.at(currentFrame));

	m_commandBuffers[currentFrame].reset(vk::CommandBufferResetFlags {});
	recordCommandBuffer(m_commandBuffers[currentFrame], imageIndex);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	vk::SubmitInfo submitInfo(m_imageAvailableSemaphores[currentFrame],
								waitDestinationStageMask,
								m_commandBuffers[currentFrame],
								m_renderFinishedSemaphores[currentFrame]);


	m_graphicsQueue.submit(submitInfo, m_inFlightFences[currentFrame]);


	vk::PresentInfoKHR presentInfo(m_renderFinishedSemaphores[currentFrame], m_swapChain, imageIndex);

	try
	{
		result = m_presentQueue.presentKHR(presentInfo);
	}
	catch (std::exception const& exc)
	{
		std::cerr << exc.what();
		//TODO - Fix
		//recreateSwapChain();
	}

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_framebufferResized)
	{
		m_framebufferResized = false;
		//TODO - Fix
		//recreateSwapChain();
	}
	else if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

Renderer_API vk::CommandBuffer VulkanRenderer::beginSingleTimeCommands()
{ 
	vk::CommandBufferAllocateInfo allocInfo { m_commandPool, vk::CommandBufferLevel::ePrimary, 1 };

	vk::CommandBuffer commandBuffer = m_device.allocateCommandBuffers(allocInfo).front();

	vk::CommandBufferBeginInfo beginInfo { vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

	commandBuffer.begin(beginInfo);
	return commandBuffer;
}

Renderer_API void VulkanRenderer::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
    commandBuffer.end();
	vk::SubmitInfo submitInfo { {}, {}, commandBuffer };
	m_graphicsQueue.submit(submitInfo);
	m_graphicsQueue.waitIdle();
	m_device.freeCommandBuffers(m_commandPool, commandBuffer);
}

/*--------------------------------------------------------------------------------*/

void VulkanRenderer::initVulkan()
{
	if (m_enableValidationLayers == VulkanRendererValidationLayerLevel::eEnabled)
	{
		createDebugMessenger();
	}
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createRenderPass();
	createGraphicsPipeline();
	createUiGraphicsPipeline();
	createCommandPool();
	createFramebuffer();


	createVertexBuffer();
	createIndexBuffer();
	createCommandBuffers();
	createSyncObjects();

	updateGraphicPipelineRecourses();

}

bool VulkanRenderer::checkDeviceExtensionSupport(const vk::PhysicalDevice &device)
{
	const std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

	std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

	for (const auto &extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
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
		// TODO - Change it to something independent of iostream
		printLog("GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.");
		exit(1);
	}

	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags{vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
														vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
														vk::DebugUtilsMessageSeverityFlagBitsEXT::eError};

	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags{vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
													   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
													   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation};

	vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{{}, severityFlags, messageTypeFlags, &debugCallback};

	m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

void VulkanRenderer::pickPhysicalDevice()
{

	std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();

	if (devices.empty())
	{
		throw std::runtime_error("Failed to find GPU's with Vulkan support!");
	}

	for (const auto &device : devices)
	{
		// Check if devices contain all required functionality
		// Pick first that fullfil condtion
		if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		{
			if (isDeviceSuitable(device))
			{
				printLog("Pick physical device: " + std::string(device.getProperties().deviceName.data(), device.getProperties().deviceName.size()));
				m_physicalDevice = device;
				break;
			}
		}
	}
}

bool VulkanRenderer::isDeviceSuitable(const vk::PhysicalDevice &device)
{

	QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(device, m_surface);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::querySwapChainSupport(device, m_surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

void VulkanRenderer::createLogicalDevice()
{
	QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(m_physicalDevice, m_surface);
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	std::set<uint32_t> uniqueQueueFamilies{indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority = 1.0F;
	for (const auto &queueFamily : uniqueQueueFamilies)
	{
		vk::DeviceQueueCreateInfo deviceQueueCreateInfo{vk::DeviceQueueCreateFlags{}, queueFamily, 1, &queuePriority};

		queueCreateInfos.push_back(deviceQueueCreateInfo);
	}

	vk::DeviceCreateInfo createInfo{vk::DeviceCreateFlags{}, queueCreateInfos, {}, m_deviceExtensions, {}};

	if (m_enableValidationLayers == VulkanRendererValidationLayerLevel::eEnabled)
	{
		std::array<const char *const, 1> deviceValidationLayers{{"VK_LAYER_KHRONOS_validation"}};
		createInfo.setPEnabledLayerNames(deviceValidationLayers);
	}
	else
	{
		//createInfo.setPEnabledLayerNames({});
	}

	m_device = m_physicalDevice.createDevice(createInfo);

	m_graphicsQueue = m_device.getQueue(indices.graphicsFamily.value(), 0);
	m_presentQueue = m_device.getQueue(indices.presentFamily.value(), 0);
}

void VulkanRenderer::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::querySwapChainSupport(m_physicalDevice, m_surface);

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	// TODO - Image count == 2?
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(m_physicalDevice, m_surface);

	std::array<uint32_t, 2> queueFamilyIndices{indices.graphicsFamily.value(), indices.presentFamily.value()};

	vk::SharingMode imageSharingMode = (indices.graphicsFamily != indices.presentFamily) ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;

	vk::SwapchainCreateInfoKHR createInfo{vk::SwapchainCreateFlagsKHR(),
										  m_surface,
										  imageCount,
										  surfaceFormat.format,
										  surfaceFormat.colorSpace,
										  extent,
										  1,
										  vk::ImageUsageFlagBits::eColorAttachment,
										  imageSharingMode,
										  queueFamilyIndices,
										  swapChainSupport.capabilities.currentTransform,
										  vk::CompositeAlphaFlagBitsKHR::eOpaque,
										  presentMode,
										  VK_TRUE,
										  VK_NULL_HANDLE};

	m_swapChain = m_device.createSwapchainKHR(createInfo);
	m_swapChainImages = m_device.getSwapchainImagesKHR(m_swapChain);
	m_uiSwapchainImages = m_device.getSwapchainImagesKHR(m_swapChain);
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;

	createSwapchainImageViews();
}

vk::SurfaceFormatKHR VulkanRenderer::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) const
{
	for (const auto &availableFormat : availableFormats)
	{
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
			availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR VulkanRenderer::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) const
{
	for (const auto &availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
		{
			return availablePresentMode;
		}
	}

	return vk::PresentModeKHR(VK_PRESENT_MODE_FIFO_KHR);
}

vk::Extent2D VulkanRenderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) const
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width = m_swapchainWidth;
		int height = m_swapchainHeight;

		VkExtent2D actualExtent{static_cast<uint32_t>(m_swapchainWidth), static_cast<uint32_t>(m_swapchainHeight)};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void VulkanRenderer::createSwapchainImageViews()
{
	for (const auto &swapChainImage : m_swapChainImages)
	{
		vk::ImageViewCreateInfo createInfo{
			vk::ImageViewCreateFlags{},
			swapChainImage,
			vk::ImageViewType::e2D,
			m_swapChainImageFormat,
			vk::ComponentMapping{vk::ComponentSwizzle::eIdentity,
								 vk::ComponentSwizzle::eIdentity,
								 vk::ComponentSwizzle::eIdentity,
								 vk::ComponentSwizzle::eIdentity},
			vk::ImageSubresourceRange{vk::ImageAspectFlags{vk::ImageAspectFlagBits::eColor}, 0, 1, 0, 1}};

		m_swapChainImageViews.emplace_back(m_device.createImageView(createInfo));
	}

	for (const auto &swapChainImage : m_uiSwapchainImages)
	{
		vk::ImageViewCreateInfo createInfo{
			vk::ImageViewCreateFlags{},
			swapChainImage,
			vk::ImageViewType::e2D,
			m_swapChainImageFormat,
			vk::ComponentMapping{vk::ComponentSwizzle::eIdentity,
								 vk::ComponentSwizzle::eIdentity,
								 vk::ComponentSwizzle::eIdentity,
								 vk::ComponentSwizzle::eIdentity},
			vk::ImageSubresourceRange{vk::ImageAspectFlags{vk::ImageAspectFlagBits::eColor}, 0, 1, 0, 1}};

		m_uiSwapchainImageViews.emplace_back(m_device.createImageView(createInfo));
	}
}

void VulkanRenderer::createRenderPass()
{
	vk::AttachmentDescription colorAttachment{vk::AttachmentDescriptionFlags{},
											  m_swapChainImageFormat,
											  vk::SampleCountFlagBits::e1,
											  vk::AttachmentLoadOp::eClear,
											  vk::AttachmentStoreOp::eStore,
											  vk::AttachmentLoadOp::eDontCare,
											  vk::AttachmentStoreOp::eDontCare,
											  vk::ImageLayout::eUndefined,
											  vk::ImageLayout::ePresentSrcKHR};

	vk::AttachmentDescription depthAttachment{vk::AttachmentDescriptionFlags{},
											  findDepthFormat(),
											  vk::SampleCountFlagBits::e1,
											  vk::AttachmentLoadOp::eClear,
											  vk::AttachmentStoreOp::eDontCare,
											  vk::AttachmentLoadOp::eDontCare,
											  vk::AttachmentStoreOp::eDontCare,
											  vk::ImageLayout::eUndefined,
											  vk::ImageLayout::eDepthStencilAttachmentOptimal};

	vk::AttachmentReference colorAttachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal};
	vk::AttachmentReference depthAttachmentRef{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

	vk::SubpassDescription subpass{vk::SubpassDescriptionFlags{}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentRef, {}, &depthAttachmentRef, {}};

	vk::SubpassDependency dependency{VK_SUBPASS_EXTERNAL,
									 0,
									 vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
									 vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
									 vk::AccessFlagBits::eNoneKHR,
									 vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite};

	std::array<vk::AttachmentDescription, 2> attachments{colorAttachment, depthAttachment};

	// attachment [0] -> color
	// atachment  [1] -> depth
	vk::RenderPassCreateInfo renderPassInfo{vk::RenderPassCreateFlags{}, attachments, subpass, dependency};

	m_renderPass = m_device.createRenderPass(renderPassInfo);
}

vk::Format VulkanRenderer::findDepthFormat() const
{
	return findSupportedFormat({vk::Format::eD32Sfloat,
								vk::Format::eD32SfloatS8Uint,
								vk::Format::eD24UnormS8Uint},
							   vk::ImageTiling::eOptimal,
							   vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

vk::Format VulkanRenderer::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
{
	for (const auto &format : candidates)
	{
		vk::FormatProperties props{m_physicalDevice.getFormatProperties(format)};

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

void VulkanRenderer::createGraphicsPipeline()
{
	createTextureSampler();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSetLayout(); // must stay in pipline creation

	auto vertShaderCode = st::renderer::Shader::readFile("Assets/Shaders/vert.spv");
	auto fragShaderCode = st::renderer::Shader::readFile("Assets/Shaders/frag.spv");

	vk::ShaderModule vertShaderModule = st::renderer::Shader::createShaderModule(m_device, vertShaderCode);
	vk::ShaderModule fragShaderModule = st::renderer::Shader::createShaderModule(m_device, fragShaderCode);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{{}, vk::ShaderStageFlagBits::eVertex, vertShaderModule, "main"};

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{{}, vk::ShaderStageFlagBits::eFragment, fragShaderModule, "main"};

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{vertShaderStageInfo, fragShaderStageInfo};

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{{}, bindingDescription, attributeDescriptions};

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{vk::PipelineInputAssemblyStateCreateFlags{}, vk::PrimitiveTopology::eTriangleList, VK_FALSE};

	vk::PipelineViewportStateCreateInfo viewportState{vk::PipelineViewportStateCreateFlags{}, 1, {}, 1, {}};

	vk::PipelineRasterizationStateCreateInfo rasterizer{vk::PipelineRasterizationStateCreateFlags{},
														VK_FALSE,
														VK_FALSE,
														vk::PolygonMode::eFill,
														vk::CullModeFlagBits::eBack,
														vk::FrontFace::eCounterClockwise,
														VK_FALSE,
														0.0f,
														0.0f,
														0.0f,
														1.0F};

	vk::PipelineMultisampleStateCreateInfo multisampling{vk::PipelineMultisampleStateCreateFlags{}, vk::SampleCountFlagBits::e1, VK_FALSE};

	vk::PipelineDepthStencilStateCreateInfo depthStencil{{}, true, true, vk::CompareOp::eLess, false, false};

	m_dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

	m_pipelineDynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo{{}, m_dynamicStateEnables};

	vk::PipelineColorBlendAttachmentState colorBlendAttachment{VK_FALSE,
															   vk::BlendFactor::eZero,
															   vk::BlendFactor::eZero,
															   vk::BlendOp::eAdd,
															   vk::BlendFactor::eZero,
															   vk::BlendFactor::eZero,
															   vk::BlendOp::eAdd,
															   vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
																   vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};

	vk::PipelineColorBlendStateCreateInfo colorBlending{
		vk::PipelineColorBlendStateCreateFlags{},
		VK_FALSE,
		vk::LogicOp::eCopy,
		colorBlendAttachment,
		{0.0f, 0.0f, 0.0f, 0.0f}};

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{vk::PipelineLayoutCreateFlags{}, m_descriptorSetLayout};

	m_pipelineLayout = m_device.createPipelineLayout(pipelineLayoutInfo);

	vk::GraphicsPipelineCreateInfo pipelineInfo{vk::PipelineCreateFlags{},
												shaderStages,
												&vertexInputInfo,
												&inputAssembly,
												{},
												&viewportState,
												&rasterizer,
												&multisampling,
												&depthStencil,
												&colorBlending,
												&m_pipelineDynamicStateCreateInfo,
												m_pipelineLayout,
												m_renderPass};

	m_pipelineCache = m_device.createPipelineCache(vk::PipelineCacheCreateInfo());
	m_graphicsPipeline = m_device.createGraphicsPipeline(m_pipelineCache, pipelineInfo).value;

	// Note VkShaderModule is passed into pipline and are not longer available trought object they are used to create
	// If ther are used later, then they must not be destroyed
	m_device.destroy(fragShaderModule);
	m_device.destroy(vertShaderModule);
}

void VulkanRenderer::createTextureSampler()
{
	vk::PhysicalDeviceProperties properties = m_physicalDevice.getProperties();

	vk::SamplerCreateInfo sampleInfo{
		{},
		vk::Filter::eLinear,
		vk::Filter::eLinear,
		vk::SamplerMipmapMode::eLinear,
		vk::SamplerAddressMode::eRepeat,
		vk::SamplerAddressMode::eRepeat,
		vk::SamplerAddressMode::eRepeat,
		0.0f,
		false,
		properties.limits.maxSamplerAnisotropy,
		false,
		vk::CompareOp::eAlways,
		0.0f,
		0.0f,
		vk::BorderColor::eIntOpaqueBlack,
		false,
	};

	m_textureSampler = m_device.createSampler(sampleInfo);
}

void VulkanRenderer::createUniformBuffers()
{
	const VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		createBuffer(bufferSize,
					 vk::BufferUsageFlagBits::eUniformBuffer,
					 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
					 m_uniformBuffers[i],
					 m_uniformBuffersMemory[i]);
	}
}

void VulkanRenderer::createDescriptorPool()
{
	std::array<vk::DescriptorPoolSize, 2> poolsSize {
		vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT * 2},
		vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, MAX_FRAMES_IN_FLIGHT * 2} 
	};

	const vk::DescriptorPoolCreateInfo poolInfo{{}, MAX_FRAMES_IN_FLIGHT * 2, poolsSize};
	m_primitiveDescriptorPool = m_device.createDescriptorPool(poolInfo);
}

void VulkanRenderer::createDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex };

	vk::DescriptorSetLayoutBinding samplerLayoutBinding { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment };

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings { uboLayoutBinding, samplerLayoutBinding };
	vk::DescriptorSetLayoutCreateInfo layoutInfo { {}, bindings };

	m_descriptorSetLayout = m_device.createDescriptorSetLayout(layoutInfo);
}

void VulkanRenderer::updateGraphicPipelineRecourses()
{
	int texWidth = 0;
	int texHeight = 0;
	int texChannels = 0;
	stbi_uc* pixels = stbi_load("Assets/Textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	std::span<std::byte> pixelsByte { reinterpret_cast<std::byte*>(pixels), static_cast<std::span<std::byte>::size_type>(texWidth * texHeight * 4) };
	Texture texture { texWidth, texHeight, texChannels, pixelsByte };	

	createTextureImage(texture, m_textureImage, textureImageMemory);
	createTextureImageView(m_textureImage, m_textureImageView);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vk::DescriptorBufferInfo bufferInfo { m_uniformBuffers.at(i), 0, sizeof(UniformBufferObject) };
		vk::DescriptorImageInfo imageInfo { m_textureSampler, m_textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal };


		std::array<vk::WriteDescriptorSet, 2> graphicDescriptorWrites {
			vk::WriteDescriptorSet { m_descriptorSets.at(i), 0, 0, vk::DescriptorType::eUniformBuffer,        {},        bufferInfo, {}},
			vk::WriteDescriptorSet { m_descriptorSets.at(i), 1, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo, {},         {}}
		};

		m_device.updateDescriptorSets(graphicDescriptorWrites, {});
	}
}

uint32_t VulkanRenderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
	vk::PhysicalDeviceMemoryProperties memProperties = m_physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanRenderer::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::BufferImageCopy region {
		0, 0, 0, { vk::ImageAspectFlagBits::eColor, 0, 0, 1 },
			{ 0, 0, 0 },
			{ width, height, 1 }
	};

	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

	endSingleTimeCommands(commandBuffer);
}

void VulkanRenderer::createUiGraphicsPipeline()
{
	std::array<vk::DescriptorPoolSize, 1> poolsSize {
		vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, MAX_FRAMES_IN_FLIGHT * 2} 
	};

	const vk::DescriptorPoolCreateInfo poolInfo{{}, MAX_FRAMES_IN_FLIGHT * 2, poolsSize};
	m_uiDescriptorPool = m_device.createDescriptorPool(poolInfo);


	vk::AttachmentDescription colorAttachment{vk::AttachmentDescriptionFlags{},
											  m_swapChainImageFormat,
											  vk::SampleCountFlagBits::e1,
											  vk::AttachmentLoadOp::eDontCare,
											  vk::AttachmentStoreOp::eStore,
											  vk::AttachmentLoadOp::eDontCare,
											  vk::AttachmentStoreOp::eDontCare,
											  vk::ImageLayout::eUndefined,
											  vk::ImageLayout::ePresentSrcKHR};


	vk::AttachmentReference colorAttachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal};


	 vk::SubpassDescription subpass(
        {},                             	// flags
        vk::PipelineBindPoint::eGraphics, 	// pipelineBindPoint
        0,                               	// inputAttachmentCount
        {},                         		// pInputAttachments
        1,                               	// colorAttachmentCount
        &colorAttachmentRef,             	// pColorAttachments
        {},                         		// pResolveAttachments
        {},                         		// pDepthStencilAttachment
        0,                               	// preserveAttachmentCount
        {}                          		// pPreserveAttachments
    );


    vk::SubpassDependency dependency(
        VK_SUBPASS_EXTERNAL,               // srcSubpass
        0,                                 // dstSubpass
        vk::PipelineStageFlagBits::eColorAttachmentOutput, // srcStageMask
        vk::PipelineStageFlagBits::eColorAttachmentOutput, // dstStageMask
        {},                                // srcAccessMask
        vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite // dstAccessMask
    );

	std::array<vk::AttachmentDescription, 1> attachments{colorAttachment};
	vk::RenderPassCreateInfo renderPassInfo{{}, attachments, subpass, dependency};


	m_uiRenderPass = m_device.createRenderPass(renderPassInfo);
}

void VulkanRenderer::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(m_physicalDevice, m_surface);

		//TODO This is a graphic commandPoll
	vk::CommandPoolCreateInfo poolInfo { vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueFamilyIndices.graphicsFamily.value() };

	m_commandPool = m_device.createCommandPool(poolInfo);
}

void VulkanRenderer::createFramebuffer()
{
	createDepthResources();

	m_swapchainFramebuffers.reserve(m_swapChainImageViews.size());
	for (const auto& swapChainImageView : m_swapChainImageViews)
	{
		std::array<vk::ImageView, 2> attachments { swapChainImageView, m_depthImageView };

		vk::FramebufferCreateInfo framebufferInfo { vk::FramebufferCreateFlags {},
													m_renderPass,
													attachments,
													m_swapChainExtent.width,
													m_swapChainExtent.height,
													1 };

		m_swapchainFramebuffers.emplace_back(m_device.createFramebuffer(framebufferInfo));
	}


	m_uiSwapchainFramebuffers.reserve(m_uiSwapchainImageViews.size());
	for (const auto& swapChainImageView : m_uiSwapchainImageViews)
	{
		std::array<vk::ImageView, 1> attachments { swapChainImageView};

		vk::FramebufferCreateInfo framebufferInfo { vk::FramebufferCreateFlags {},
													m_uiRenderPass,
													attachments,
													m_swapChainExtent.width,
													m_swapChainExtent.height,
													1 };

		m_uiSwapchainFramebuffers.emplace_back(m_device.createFramebuffer(framebufferInfo));
	}
}

void VulkanRenderer::createDepthResources()
{
	vk::Format depthFormat = findDepthFormat();

	createImage(m_swapChainExtent.width,
							m_swapChainExtent.height,
							depthFormat,
							vk::ImageTiling::eOptimal,
							vk::ImageUsageFlagBits::eDepthStencilAttachment,
							vk::MemoryPropertyFlagBits::eDeviceLocal,
							m_depthImage,
							m_depthImageMemory);

	m_depthImageView = createImageView(m_depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

void VulkanRenderer::createVertexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(planeVertexes[0]) * planeVertexes.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;


	createBuffer(bufferSize,
									vk::BufferUsageFlagBits::eTransferSrc,
									vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
									stagingBuffer,
									stagingBufferMemory);

	void* lineData = m_device.mapMemory(stagingBufferMemory, 0, bufferSize);
	memcpy(lineData, planeVertexes.data(), (size_t)bufferSize); //vertices should fullfil trival object specyfication?
	m_device.unmapMemory(stagingBufferMemory);

	createBuffer(bufferSize,
									vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
									vk::MemoryPropertyFlagBits::eDeviceLocal,
									m_planeVertexBuffer,
									m_planeVertexBufferMemory);

	copyBuffer(stagingBuffer, m_planeVertexBuffer, bufferSize);

	m_device.destroyBuffer(stagingBuffer);
	m_device.freeMemory(stagingBufferMemory);


	std::vector<vk::DescriptorSetLayout> graphicLayouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
	const vk::DescriptorSetAllocateInfo graphicAllocInfo { m_primitiveDescriptorPool, graphicLayouts };
	m_descriptorSets = m_device.allocateDescriptorSets(graphicAllocInfo);
}

void VulkanRenderer::createIndexBuffer()
{
	vk::DeviceSize planeBufferSize = sizeof(planeIndices[0]) * planeIndices.size();

	vk::Buffer planetagingBuffer;
	vk::DeviceMemory planetagingBufferMemory;
	createBuffer(planeBufferSize,
									vk::BufferUsageFlagBits::eTransferSrc,
									vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
									planetagingBuffer,
									planetagingBufferMemory);

	void* planeData = m_device.mapMemory(planetagingBufferMemory, 0, planeBufferSize);
	memcpy(planeData, planeIndices.data(), (size_t)planeBufferSize);
	m_device.unmapMemory(planetagingBufferMemory);

	createBuffer(planeBufferSize,
									vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
									vk::MemoryPropertyFlagBits::eDeviceLocal,
									m_planeIndexBuffer,
									m_planeIndexBufferMemory);

	copyBuffer(planetagingBuffer, m_planeIndexBuffer, planeBufferSize);

	m_device.destroyBuffer(planetagingBuffer);
	m_device.freeMemory(planetagingBufferMemory);
}

void VulkanRenderer::createCommandBuffers()
{
	vk::CommandBufferAllocateInfo cmdAllocInfo { m_commandPool,
												vk::CommandBufferLevel::ePrimary,
												static_cast<uint32_t>(m_swapchainFramebuffers.size()) };

	vk::CommandBufferAllocateInfo uiCmdAllocInfo { m_commandPool,
												vk::CommandBufferLevel::ePrimary,
												static_cast<uint32_t>(m_uiSwapchainFramebuffers.size()) };

	m_commandBuffers 	= m_device.allocateCommandBuffers(cmdAllocInfo);
	m_uiCommandBuffers 	= m_device.allocateCommandBuffers(uiCmdAllocInfo);

}

void VulkanRenderer::createSyncObjects()
{
	m_imageAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
	m_uiAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_imageAvailableSemaphores.emplace_back(m_device.createSemaphore(vk::SemaphoreCreateInfo {}));
		m_uiAvailableSemaphores.emplace_back(m_device.createSemaphore(vk::SemaphoreCreateInfo {}));
		m_renderFinishedSemaphores.emplace_back(m_device.createSemaphore(vk::SemaphoreCreateInfo {}));
		m_inFlightFences.emplace_back(m_device.createFence(vk::FenceCreateInfo { vk::FenceCreateFlagBits::eSignaled }));
	}
}

void VulkanRenderer::updateUniformBuffer(uint32_t currentImage)
{
	UniformBufferObject ubo {};
	ubo.model = st::math::Matrix4x4::indentityMatrix();
	ubo.model.convertToColumnMajor();

	ubo.view = camera.getViewMatrix();
	ubo.view.convertToColumnMajor();

	ubo.proj = camera.getProjectionMatrix(45.0F,
											(m_swapChainExtent.width / static_cast<float>(m_swapChainExtent.height)),
											0.1F,
											100.0F);
	ubo.proj.convertToColumnMajor();


	void* data = m_device.mapMemory(m_uniformBuffersMemory.at(currentImage), 0, sizeof(ubo));
	memcpy(data, &ubo, sizeof(ubo));
	m_device.unmapMemory(m_uniformBuffersMemory.at(currentImage));
}

void VulkanRenderer::recordCommandBuffer(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
	commandBuffer.begin(vk::CommandBufferBeginInfo {});

	const vk::ClearColorValue colorClean {
		std::array<float, 4> {0.0F, 0.0F, 0.0F, 1.0F}
	};
	const vk::ClearDepthStencilValue depthClean { 1.0F, 0 };

	std::array<vk::ClearValue, 2> clearValues { colorClean, depthClean };

	//Draw primitive
	vk::Extent2D swapChainExtent = m_swapChainExtent;
	vk::RenderPassBeginInfo renderPassInfo { m_renderPass,
												m_swapchainFramebuffers[imageIndex],
												vk::Rect2D((0, 0), swapChainExtent),
												clearValues };

	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	vk::Viewport viewport { 0.0F, 0.0F, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0F, 1.0F };

	vk::Rect2D scissor {
		{0, 0},
		swapChainExtent
	};
	commandBuffer.setViewport(0, 1, &viewport);
	commandBuffer.setScissor(0, 1, &scissor);


	//-------------------Draw all objects----------------------------------
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline);

	vk::Buffer vertexBuffers[] = { m_planeVertexBuffer };
	vk::DeviceSize offsets[] = { 0 };

	commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
	commandBuffer.bindIndexBuffer(m_planeIndexBuffer, 0, vk::IndexType::eUint32);

	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
										m_pipelineLayout,
										0,
										m_descriptorSets.at(currentFrame),
										{});

	commandBuffer.drawIndexed(planeIndices.size(), 1, 0, 0, 0);
	

	commandBuffer.endRenderPass();
	commandBuffer.end();
}

void VulkanRenderer::createBuffer(vk::DeviceSize size,
                                  vk::BufferUsageFlags usage,
                                  vk::MemoryPropertyFlags properties,
                                  vk::Buffer &buffer,
                                  vk::DeviceMemory &bufferMemory) const
{

	vk::BufferCreateInfo bufferInfo{{}, size, usage, vk::SharingMode::eExclusive};

	buffer = m_device.createBuffer(bufferInfo);

	vk::MemoryRequirements memoryRequirements = m_device.getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo allocInfo{memoryRequirements.size, findMemoryType(memoryRequirements.memoryTypeBits, properties)};

	bufferMemory = m_device.allocateMemory(allocInfo);

	m_device.bindBufferMemory(buffer, bufferMemory, 0);
}

void VulkanRenderer::createImage(uint32_t width,
				 uint32_t height,
				 vk::Format format,
				 vk::ImageTiling tiling,
				 vk::ImageUsageFlags usage,
				 vk::MemoryPropertyFlags properties,
				 vk::Image& image,
				 vk::DeviceMemory& imageMemory) const
{

	vk::ImageCreateInfo imageInfo {
		{},
        vk::ImageType::e2D,          format, { width, height, 1 },
        1, 1, vk::SampleCountFlagBits::e1, tiling,
		usage, vk::SharingMode::eExclusive, {},
        vk::ImageLayout::eUndefined
	};

	image = m_device.createImage(imageInfo);

	vk::MemoryRequirements m_memRequirements = m_device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo { m_memRequirements.size, findMemoryType(m_memRequirements.memoryTypeBits, properties) };

	imageMemory = m_device.allocateMemory(allocInfo);

	m_device.bindImageMemory(image, imageMemory, 0);
}

vk::ImageView VulkanRenderer::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags) const
{
	vk::ImageViewCreateInfo viewInfo {
		{},
		image, vk::ImageViewType::e2D, format, {},
		{ aspectFlags, 0, 1, 0, 1 },
		{}
	};

	return m_device.createImageView(viewInfo);
}

void VulkanRenderer::createTextureImage(Texture &texture, vk::Image &textureImage, vk::DeviceMemory &textureImageMemory)
{
	vk::DeviceSize imageSize = texture.textureWidth * texture.textureHeight * 4;


		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;

		createBuffer(imageSize,
									 vk::BufferUsageFlagBits::eTransferSrc,
									 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
									 stagingBuffer,
									 stagingBufferMemory);

		void* data = m_device.mapMemory(stagingBufferMemory, 0, imageSize);
		memcpy(data, texture.pixels.data(), static_cast<size_t>(imageSize));
		m_device.unmapMemory(stagingBufferMemory);


		createImage(texture.textureWidth,
								   texture.textureHeight,
								   vk::Format::eR8G8B8A8Srgb,
								   vk::ImageTiling::eOptimal,
								   vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
								   vk::MemoryPropertyFlagBits::eDeviceLocal,
								   textureImage,
								   textureImageMemory);

		transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

		copyBufferToImage(stagingBuffer, textureImage, texture.textureWidth, texture.textureHeight);

		transitionImageLayout(textureImage,
											 vk::Format::eR8G8B8A8Srgb,
											 vk::ImageLayout::eTransferDstOptimal,
											 vk::ImageLayout::eShaderReadOnlyOptimal);

		m_device.destroyBuffer(stagingBuffer);
		m_device.freeMemory(stagingBufferMemory);
}

void VulkanRenderer::createTextureImageView(vk::Image &textureImage, vk::ImageView &textureImageView)
{
	textureImageView = createImageView(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

}

void VulkanRenderer::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

		vk::ImageMemoryBarrier barrier {
			vk::AccessFlagBits::eNone, vk::AccessFlagBits::eNone, oldLayout, newLayout,
			VK_QUEUE_FAMILY_IGNORED,   VK_QUEUE_FAMILY_IGNORED,   image,     {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
		};

		vk::PipelineStageFlags sourceStage {};
		vk::PipelineStageFlags destinationStage {};

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eNone;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eNone;
			barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);

		endSingleTimeCommands(commandBuffer);
}

vk::VertexInputBindingDescription Vertex::getBindingDescription()
{
	vk::VertexInputBindingDescription bindingDescription{
		0,
		sizeof(Vertex),
		vk::VertexInputRate::eVertex};

	return bindingDescription;
}

std::array<vk::VertexInputAttributeDescription, 4> Vertex::getAttributeDescriptions()
{
	std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions{
		vk::VertexInputAttributeDescription{
			0,
			0,
			vk::Format::eR32G32B32Sfloat,
			static_cast<uint32_t>(offsetof(Vertex, m_pos))},
		vk::VertexInputAttributeDescription{
			1,
			0,
			vk::Format::eR32G32Sfloat,
			static_cast<uint32_t>(offsetof(Vertex, m_texCoord))},
		vk::VertexInputAttributeDescription{
			2,
			0,
			vk::Format::eR32G32B32Sfloat,
			static_cast<uint32_t>(offsetof(Vertex, m_color))},
		vk::VertexInputAttributeDescription{
			3,
			0,
			vk::Format::eR32G32B32Sfloat,
			static_cast<uint32_t>(offsetof(Vertex, m_normal))}};

	return attributeDescriptions;
}

void VulkanRenderer::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	vk::CommandBufferAllocateInfo allocInfo { m_commandPool, vk::CommandBufferLevel::ePrimary, 1 };

	auto commandBuffer = m_device.allocateCommandBuffers(allocInfo);

	vk::CommandBufferBeginInfo beginInfo { vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

	commandBuffer.at(0).begin(beginInfo);

	vk::BufferCopy copyRegin { 0, 0, size };

	commandBuffer.at(0).copyBuffer(srcBuffer, dstBuffer, 1, &copyRegin);

	commandBuffer.at(0).end();

	vk::SubmitInfo submitInfo { {}, {}, commandBuffer };

	m_graphicsQueue.submit(1, &submitInfo, {}); //TODO GraphicQueue should copy buffers?
	m_graphicsQueue.waitIdle();

	m_device.freeCommandBuffers(m_commandPool, commandBuffer);
}