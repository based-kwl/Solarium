#include "SwapChain.hpp"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace Solarium {

	SwapChain::SwapChain(Device& deviceRef, vk::Extent2D extent)
		: device{ deviceRef }, windowExtent{ extent } 
	{
		createSwapChain();
		createImageViews();
		createRenderPass();
		createDepthResources();
		createFramebuffers();
		createSyncObjects();
	}

	SwapChain::~SwapChain() 
	{
		for (auto imageView : swapChainImageViews) 
		{
			device.device().destroyImageView(imageView);
		}
		swapChainImageViews.clear();

		if (swapChain) {
			device.device().destroySwapchainKHR(swapChain);
			swapChain = nullptr;
		}

		for (int i = 0; i < depthImages.size(); i++) {
			device.device().destroyImageView(depthImageViews[i]);
			device.device().destroyImage(depthImages[i]);
			device.device().freeMemory(depthImageMemorys[i]);
		}

		for (auto framebuffer : swapChainFramebuffers) {
			device.device().destroyFramebuffer(framebuffer);
		}

		device.device().destroyRenderPass(renderPass);

		// cleanup synchronization objects

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			device.device().destroySemaphore(renderFinishedSemaphores[i]);
			device.device().destroySemaphore(imageAvailableSemaphores[i]);
			device.device().destroyFence(inFlightFences[i]);
		}
	}

	vk::Result SwapChain::acquireNextImage(uint32_t* imageIndex) 
  {
		device.device().waitForFences(inFlightFences[currentFrame], true, std::numeric_limits<uint64_t>::max());
		vk::Result result = device.device().acquireNextImageKHR(swapChain, std::numeric_limits<uint64_t>::max(),imageAvailableSemaphores[currentFrame], nullptr, imageIndex);
		return result;
	}
	vk::Result SwapChain::submitCommandBuffers(
		const vk::CommandBuffer* buffers, uint32_t* imageIndex) {
		if (imagesInFlight[*imageIndex]) {
			device.device().waitForFences(imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
		}
		imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

		vk::SubmitInfo submitInfo = {};

		vk::Semaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = buffers;

		vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		device.device().resetFences(inFlightFences[currentFrame]);
		device.graphicsQueue().submit(submitInfo, inFlightFences[currentFrame]);


		vk::PresentInfoKHR presentInfo = {};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		vk::SwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = imageIndex;

		auto result = device.presentQueue().presentKHR(presentInfo);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		return result;
	}

	void SwapChain::createSwapChain() 
	{
		SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();

		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
			imageCount > swapChainSupport.capabilities.maxImageCount) 
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR createInfo = {};
		createInfo.surface = device.surface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		QueueFamilyIndices indices = device.findPhysicalQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };


		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
			createInfo.queueFamilyIndexCount = 0;      // Optional
			createInfo.pQueueFamilyIndices = nullptr;  // Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = nullptr;


		swapChain = device.device().createSwapchainKHR(createInfo);
		if (!swapChain) {

			throw std::runtime_error("failed to create swap chain!");
		}

		// we only specified a minimum number of images in the swap chain, so the implementation is
		// allowed to create a swap chain with more. That's why we'll first query the final number of
		// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
		// retrieve the handles.
		swapChainImages = device.device().getSwapchainImagesKHR(swapChain);

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	//Needs to be reworked for 3D
	void SwapChain::createImageViews() 
	{
		swapChainImageViews.resize(swapChainImages.size());
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			vk::ImageViewCreateInfo viewInfo{};
			viewInfo.image = swapChainImages[i];
			viewInfo.viewType = vk::ImageViewType::e2D;
			viewInfo.format = swapChainImageFormat;
			viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			swapChainImageViews[i] = device.device().createImageView(viewInfo);
			if (!swapChainImageViews[i]) {
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}


	void SwapChain::createRenderPass() {
		vk::AttachmentDescription depthAttachment{};

		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = vk::SampleCountFlagBits::e1;
		depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
		depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.format = getSwapChainImageFormat();
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		vk::SubpassDependency dependency = {};

		dependency.dstSubpass = 0;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = {};
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		vk::RenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		renderPass = device.device().createRenderPass(renderPassInfo);
		if (!renderPass) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void SwapChain::createFramebuffers() 
	{
		swapChainFramebuffers.resize(imageCount());

		for (size_t i = 0; i < imageCount(); i++) {
			std::array<vk::ImageView, 2> attachments = { swapChainImageViews[i], depthImageViews[i] };

			vk::Extent2D swapChainExtent = getSwapChainExtent();
			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			swapChainFramebuffers[i] = device.device().createFramebuffer(framebufferInfo);
			
			if(!swapChainFramebuffers[i]) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void SwapChain::createDepthResources() {
		vk::Format depthFormat = findDepthFormat();
		vk::Extent2D swapChainExtent = getSwapChainExtent();

		depthImages.resize(imageCount());
		depthImageMemorys.resize(imageCount());
		depthImageViews.resize(imageCount());


		for (int i = 0; i < depthImages.size(); i++) {
			vk::ImageCreateInfo imageInfo{};
			imageInfo.imageType = vk::ImageType::e2D;
			imageInfo.extent.width = swapChainExtent.width;
			imageInfo.extent.height = swapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = vk::ImageTiling::eOptimal;
			imageInfo.initialLayout = vk::ImageLayout::eUndefined;
			imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
			imageInfo.samples = vk::SampleCountFlagBits::e1;
			imageInfo.sharingMode = vk::SharingMode::eExclusive;
			imageInfo.flags = {};
			device.createImageWithInfo(
				imageInfo,
				vk::MemoryPropertyFlagBits::eDeviceLocal,
				depthImages[i],
				depthImageMemorys[i]);

			vk::ImageViewCreateInfo viewInfo{};
			viewInfo.image = depthImages[i];
			viewInfo.viewType = vk::ImageViewType::e2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			depthImageViews[i] = device.device().createImageView(viewInfo);
			if (!depthImageViews[i]) {
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	void SwapChain::createSyncObjects() 
	{
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(imageCount());
		vk::SemaphoreCreateInfo semaphoreInfo = {};
		vk::FenceCreateInfo fenceInfo = {};
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			imageAvailableSemaphores[i] = device.device().createSemaphore(semaphoreInfo);
			renderFinishedSemaphores[i] = device.device().createSemaphore(semaphoreInfo);
			inFlightFences[i] = device.device().createFence(fenceInfo);
			if (!imageAvailableSemaphores[i] || !renderFinishedSemaphores[i] || !inFlightFences[i]) {

				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(
		const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
				availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {

				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	vk::PresentModeKHR SwapChain::chooseSwapPresentMode(
		const std::vector<vk::PresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				std::cout << "Present mode: Mailbox" << std::endl;
				return availablePresentMode;
			}
		}

		// for (const auto &availablePresentMode : availablePresentModes) {
		//   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
		//     std::cout << "Present mode: Immediate" << std::endl;
		//     return availablePresentMode;
		//   }
		// }

		std::cout << "Present mode: V-Sync" << std::endl;
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			vk::Extent2D actualExtent = windowExtent;
			actualExtent.width = std::max(
				capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(
				capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	vk::Format SwapChain::findDepthFormat() {
		return device.findSupportedFormat(
			{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	}

}
