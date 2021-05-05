#pragma once

#include "vulkan/vulkan.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "BufferHelper.hpp"
#include <stb_image.h>

namespace Solarium
{

	class Texture
	{
	public:
		Texture(SwapChain* swapChain_, Device* device_);
		~Texture();
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		void createChain();
		vk::Sampler getTextureSampler() { return textureSampler; }
		vk::DeviceMemory getIndexBufferMemory() { return indexBufferMemory; }
		vk::ImageView getTextureImageView() { return textureImageView; };
		vk::Image getTextureImage() { return textureImage; }
		vk::DeviceMemory getTextureImageMemory() { return textureImageMemory; }
		std::vector<vk::ImageView> getSwapChainImageViews() { return swapChainImageViews; }

	private:
		vk::ImageView createImageView(vk::Image image, vk::Format format);
		void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);
		void createTextureImage();
		void createImageViews();
		void createTextureImageView();
		void createTextureSampler();
		void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

		vk::Sampler textureSampler;
		vk::DeviceMemory indexBufferMemory;
		vk::ImageView textureImageView;
		vk::Image textureImage;
		vk::DeviceMemory textureImageMemory;
		std::vector<vk::ImageView> swapChainImageViews;
		Device* device;
		SwapChain* swapChain;
	};
}