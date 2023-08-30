#include "Renderer.hpp"

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>
#define VK_USE_PLATFORM_ANDROID_KHR 0
#endif

#include <string>
#include <sstream>
#include <iostream>
#include <set>
#include "Shaders/Shader.hpp"
#include "StMath/StMath.hpp"
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
	return {};
}

vk::DescriptorPool VulkanRenderer::getUiDescriptorPool() const
{
	return {};
}

vk::RenderPass VulkanRenderer::getUiRenderPass() const
{
	return {};
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
		std::vector<const char *> deviceValidationLayers{{"VK_LAYER_KHRONOS_validation"}};
		createInfo.setPEnabledLayerNames(deviceValidationLayers);
	}
	else
	{
		createInfo.setPEnabledLayerNames({});
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

	auto vertShaderCode = st::renderer::Shader::readFile("../Assets/Shaders/vert.spv");
	auto fragShaderCode = st::renderer::Shader::readFile("../Assets/Shaders/frag.spv");

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
