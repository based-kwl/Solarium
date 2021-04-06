#pragma once

#include "Device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

namespace Solarium 
{

	class SwapChain 
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device& deviceRef, vk::Extent2D windowExtent);
		~SwapChain();

		SwapChain(const SwapChain&) = delete;
		void operator=(const SwapChain&) = delete;

		vk::Framebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
		vk::RenderPass getRenderPass() { return renderPass; }
		vk::ImageView getImageView(int index) { return swapChainImageViews[index]; }
		size_t imageCount() { return swapChainImages.size(); }
		vk::Format getSwapChainImageFormat() { return swapChainImageFormat; }
		vk::Extent2D getSwapChainExtent() { return swapChainExtent; }
		uint32_t width() { return swapChainExtent.width; }
		uint32_t height() { return swapChainExtent.height; }

		float extentAspectRatio() 
		{
			return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
		}
		vk::Format findDepthFormat();

		vk::Result acquireNextImage(uint32_t* imageIndex);
		vk::Result submitCommandBuffers(const vk::CommandBuffer* buffers, uint32_t* imageIndex);

	private:
		void createSwapChain();
		void createImageViews();
		void createDepthResources();
		void createRenderPass();
		void createFramebuffers();
		void createSyncObjects();
		
		std::vector<VkFramebuffer> getSwapChainFB() { return swapChainFramebuffers; }
		std::vector<VkImageView> getSwapChainImageViews() { return swapChainImageViews; }
		std::vector<VkFence> getInFlightFences() { return inFlightFences; }
		std::vector<VkFence> getImagesInFlight() { return imagesInFlight; }
		VkSwapchainKHR getSwapChain() { return swapChain; }
		std::vector<VkSemaphore> getImageSemaphores() { return imageAvailableSemaphores; }
		std::vector<VkSemaphore> getFinishedSemaphores() { return renderFinishedSemaphores; }
		size_t getCurrentFrame() { return currentFrame; }
		
		void setCurrentFrame(size_t newFrame) { currentFrame = newFrame; }
		void setImageInFlight(int index, VkFence target) { imagesInFlight[index] = target; }

	private:

		// Helper functions
		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR chooseSwapPresentMode(
			const std::vector<vk::PresentModeKHR>& availablePresentModes);
		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

		vk::Format swapChainImageFormat;
		vk::Extent2D swapChainExtent;

		std::vector<vk::Framebuffer> swapChainFramebuffers;
		vk::RenderPass renderPass;

		std::vector<vk::Image> depthImages;
		std::vector<vk::DeviceMemory> depthImageMemorys;
		std::vector<vk::ImageView> depthImageViews;
		std::vector<vk::Image> swapChainImages;
		std::vector<vk::ImageView> swapChainImageViews;

		Device& device;
		vk::Extent2D windowExtent;

		vk::SwapchainKHR swapChain;

		std::vector<vk::Semaphore> imageAvailableSemaphores;
		std::vector<vk::Semaphore> renderFinishedSemaphores;
		std::vector<vk::Fence> inFlightFences;
		std::vector<vk::Fence> imagesInFlight;
		size_t currentFrame = 0;
	};

} 
