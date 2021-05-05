#include "Texture.hpp"

namespace Solarium
{

	Texture::Texture(SwapChain* swapChain_, Device* device_) {
		device = device_;
		swapChain = swapChain_;
		createImageViews();
	}

	void Texture::createChain() {
		createTextureImage();
		createTextureImageView();
		createTextureSampler();
	}

	vk::ImageView Texture::createImageView(vk::Image image, vk::Format format)
	{
		vk::ImageViewCreateInfo viewInfo{ {}, image, vk::ImageViewType::e2D, format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0 ,1} };
		vk::ImageView imageView = device->device().createImageView(viewInfo);
		if (!imageView)
		{
			throw std::runtime_error("Failed to create texture image view");
		}
		return imageView;
	}

	void Texture::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory)
	{
		vk::ImageCreateInfo imageInfo{ {}, vk::ImageType::e2D, format, vk::Extent3D{static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1}, 1, 1, vk::SampleCountFlagBits::e1, tiling, usage, vk::SharingMode::eExclusive };

		textureImage = device->device().createImage(imageInfo);

		vk::MemoryRequirements memRequirements = device->device().getImageMemoryRequirements(image);
		vk::MemoryAllocateInfo allocInfo{ memRequirements.size, BufferHelper::findMemoryType(memRequirements.memoryTypeBits, properties, device) };

		textureImageMemory = device->device().allocateMemory(allocInfo);
		if (!textureImageMemory)
		{
			throw std::runtime_error("Failed to allocate image memory");
		}
		device->device().bindImageMemory(image, imageMemory, 0);
	}

	void Texture::createTextureImage()
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load("textures/textures.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		vk::DeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels)
		{
			throw std::runtime_error("Failed to load texture image");
		}

		BufferHelper::createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory, device);

		void* data = device->device().mapMemory(stagingBufferMemory, 0, imageSize);
		memcpy(data, pixels, static_cast<uint32_t>(imageSize));
		device->device().unmapMemory(stagingBufferMemory);
		stbi_image_free(pixels);

		createImage(texWidth, texHeight, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);
		transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		BufferHelper::copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), device);
		transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

		device->device().destroyBuffer(stagingBuffer);
		device->device().freeMemory(stagingBufferMemory);
	}

	void Texture::createImageViews()
	{
		swapChainImageViews.resize(swapChain->imageCount());
		for (uint32_t i = 0; i < swapChain->imageCount(); i++)
		{
			swapChainImageViews[i] = createImageView(swapChain->getSwapChainImages()[i], swapChain->getSwapChainImageFormat());
		}
	}

	void Texture::createTextureImageView()
	{
		textureImageView = createImageView(textureImage, vk::Format::eR8G8B8A8Srgb);
	}

	void Texture::createTextureSampler()
	{
		vk::SamplerCreateInfo samplerInfo{ {},  vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat , vk::SamplerAddressMode::eRepeat, 0.f, VK_TRUE, device->properties.limits.maxSamplerAnisotropy, VK_FALSE, vk::CompareOp::eAlways, 0.f, 0.f, vk::BorderColor::eIntOpaqueBlack, VK_FALSE };
		textureSampler = device->device().createSampler(samplerInfo);
		if (!textureSampler)
		{
			throw std::runtime_error("Failed to create texture sampler");
		}
	}

	void Texture::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		vk::CommandBuffer commandBuffer = BufferHelper::beginSingleTimeCommands(device);
		vk::ImageMemoryBarrier barrier{ {}, {}, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;
		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
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
		else
		{
			throw std::runtime_error("Invalid layout transition");
		}

		commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
		BufferHelper::endSingleTimeCommands(commandBuffer, device);
	}
}