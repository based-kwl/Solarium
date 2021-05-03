#include "Device.hpp"
// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace Solarium {

	// local callback functions
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		}

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance,
			"vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance,
			"vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	// class member functions
	Device::Device(Platform& window) : window{ window }
	{
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createCommandPool();
	}


	Device::~Device()
	{
		vkDestroyCommandPool(device_, commandPool, nullptr);
		vkDestroyDevice(device_, nullptr);

		if (enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(instance, surface_, nullptr);
		vkDestroyInstance(instance, nullptr);
	}

	void Device::createInstance() 
	{
		if (enableValidationLayers && !checkValidationLayerSupport()) 
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}

		vk::ApplicationInfo appInfo = {"Solarium", VK_MAKE_VERSION(1,0,0), "Solarium", VK_MAKE_VERSION(1,0,0), VK_API_VERSION_1_2};

		vk::InstanceCreateInfo createInfo = {{},&appInfo};

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers) 
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else 
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		instance = vk::createInstance(createInfo);

		hasGflwRequiredInstanceExtensions();
	}

	void Device::pickPhysicalDevice() 
	{
		std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
		if (devices.size() == 0) 
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		
		for (const auto& device : devices) 
		{
			if (isDeviceSuitable(device)) 
			{
				physicalDevice_ = device;
				break;
			}
		}

		if (!physicalDevice_) 
		{
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		properties = physicalDevice_.getProperties();
		std::cout << "physical device: " << properties.deviceName << std::endl;
	}

	void Device::createLogicalDevice() 
	{
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice_);

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) 
		{
			vk::DeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		vk::DeviceCreateInfo createInfo = {};

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// might not really be necessary anymore because device specific validation layers
		// have been deprecated
		if (enableValidationLayers) 
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else 
		{
			createInfo.enabledLayerCount = 0;
		}

		device_ = physicalDevice_.createDevice(createInfo);
		if (!device_)
		{
			throw std::runtime_error("failed to create logical device!");
		}
		
		graphicsQueue_ = device_.getQueue(indices.graphicsFamily,0);
		presentQueue_ = device_.getQueue(indices.presentFamily,0);
	}

	void Device::createCommandPool() 
	{
		QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

		vk::CommandPoolCreateInfo poolInfo = { vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueFamilyIndices.graphicsFamily};

		commandPool = device_.createCommandPool(poolInfo);
		if (!commandPool) 
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void Device::createSurface() { window.createWindowSurface(instance, &surface_); }

	bool Device::isDeviceSuitable(vk::PhysicalDevice device) 
	{
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) 
		{
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

		return indices.isComplete() && extensionsSupported && swapChainAdequate &&
			supportedFeatures.samplerAnisotropy;
	}

	void Device::populateDebugMessengerCreateInfo(
		VkDebugUtilsMessengerCreateInfoEXT& createInfo) 
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;  // Optional
	}

	void Device::setupDebugMessenger() 
	{
		if (!enableValidationLayers) return;
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);
		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	bool Device::checkValidationLayerSupport() 
	{

		std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
		for (const char* layerName : validationLayers) 
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

	std::vector<const char*> Device::getRequiredExtensions() 
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) 
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void Device::hasGflwRequiredInstanceExtensions() 
	{
		std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

		std::cout << "available extensions:" << std::endl;
		std::unordered_set<std::string> available;
		for (const auto& extension : extensions) 
		{
			std::cout << "\t" << extension.extensionName << std::endl;
			available.insert(extension.extensionName);
		}

		std::cout << "required extensions:" << std::endl;
		auto requiredExtensions = getRequiredExtensions();
		for (const auto& required : requiredExtensions) 
		{
			std::cout << "\t" << required << std::endl;
			if (available.find(required) == available.end()) 
			{
				throw std::runtime_error("Missing required glfw extension");
			}
		}
	}

	bool Device::checkDeviceExtensionSupport(vk::PhysicalDevice device) 
	{
		std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) 
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	QueueFamilyIndices Device::findQueueFamilies(vk::PhysicalDevice device) 
	{
		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

		int i = 0;
		for (const auto& queueFamily : queueFamilies) 
		{
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				indices.graphicsFamily = i;
				indices.graphicsFamilyHasValue = true;
			}
			vk::Bool32 presentSupport = device.getSurfaceSupportKHR(i, surface_);
			if (queueFamily.queueCount > 0 && presentSupport) 
			{
				indices.presentFamily = i;
				indices.presentFamilyHasValue = true;
			}
			if (indices.isComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}

	SwapChainSupportDetails Device::querySwapChainSupport(vk::PhysicalDevice device)
	{
		SwapChainSupportDetails details;
		details.capabilities = device.getSurfaceCapabilitiesKHR(surface_);
		details.formats = device.getSurfaceFormatsKHR(surface_);
		details.presentModes = device.getSurfacePresentModesKHR(surface_);

		return details;
	}

	vk::Format Device::findSupportedFormat(
		const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) 
	{
		for (vk::Format format : candidates) 
		{
			vk::FormatProperties props = physicalDevice_.getFormatProperties(format);


			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) 
			{
				return format;
			}
			else if (
				tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) 
			{
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}

	uint32_t Device::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice_.getMemoryProperties();
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
		{
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void Device::createBuffer(
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Buffer& buffer,
		vk::DeviceMemory& bufferMemory) 
	{
		vk::BufferCreateInfo bufferInfo{};
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		buffer = device_.createBuffer(bufferInfo);

		if (!buffer) 
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}

		vk::MemoryRequirements memRequirements = device_.getBufferMemoryRequirements(buffer);

		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		bufferMemory = device_.allocateMemory(allocInfo);

		if (VK_NULL_HANDLE) 
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		device_.bindBufferMemory(buffer, bufferMemory, 0);
	}

	vk::CommandBuffer Device::beginSingleTimeCommands() 
	{
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		vk::CommandBuffer commandBuffer = device_.allocateCommandBuffers(allocInfo)[0];

		vk::CommandBufferBeginInfo beginInfo{};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
		commandBuffer.begin(beginInfo);
		return commandBuffer;
	}

	void Device::endSingleTimeCommands(vk::CommandBuffer commandBuffer) 
	{
		commandBuffer.end();

		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		graphicsQueue_.submit(submitInfo, nullptr);
		graphicsQueue_.waitIdle();

		device_.freeCommandBuffers(commandPool, commandBuffer);
	}

	void Device::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) 
	{
		vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

		vk::BufferCopy copyRegion{0, 0, size};
		commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
		endSingleTimeCommands(commandBuffer);
	}

	void Device::copyBufferToImage(
		vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount) 
	{
		vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

		vk::BufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		region.imageOffset.x = 0;
		region.imageOffset.y = 0;
		region.imageOffset.z = 0;
		
		region.imageExtent.width  = width;
		region.imageExtent.height = height;
		region.imageExtent.depth  = 1;
		commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
		endSingleTimeCommands(commandBuffer);
	}

	void Device::createImageWithInfo(
		const vk::ImageCreateInfo& imageInfo,
		vk::MemoryPropertyFlags properties,
		vk::Image& image,
		vk::DeviceMemory& imageMemory) 
	{
		image = device_.createImage(imageInfo);
		if (!image) 
		{
			throw std::runtime_error("failed to create image!");
		}

		vk::MemoryRequirements memRequirements = device_.getImageMemoryRequirements(image);

		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		imageMemory = device_.allocateMemory(allocInfo);
		if (!imageMemory) 
		{
			throw std::runtime_error("failed to allocate image memory!");
		}
		device_.bindImageMemory(image, imageMemory, 0);
	}

}