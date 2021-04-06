#include "Engine.hpp"

namespace Solarium
{
	Engine::Engine(const char* applicationName, uint32_t width, uint32_t height)
	{
		Solarium::Logger::Log("INITIALIZING");
		_platform = new Platform(applicationName, width, height);
		device = new Device{ *_platform };
		vk::Instance instance;
		auto renderer = new VulkanRenderer(_platform, instance);
		//pipeline = new Pipeline(*device, "../../../Shaders/out/Test_shader.vert.spv", "../../../Shaders/out/Test_shader.frag.spv", Pipeline::defaultPipelineConfigInfo(1280, 720));

		swapChain = new SwapChain(*device, _platform->getExtent());
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	Engine::~Engine()
	{
		vkDestroyPipelineLayout(device->device(), pipelineLayout, nullptr);
	}

	void Engine::Run()
	{
		while (!glfwWindowShouldClose(_platform->GetWindow()))
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(device->device());
	}

	void Engine::OnLoop(const uint32_t deltaTime)
	{

	}

	void Engine::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(device->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout.");
		}
	}

	void Engine::createPipeline()
	{
		auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(swapChain->width(), swapChain->height());
		pipelineConfig.renderPass = swapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipeline = std::make_unique<Pipeline>(*device, "../../../Shaders/out/Test_shader.vert.spv", "../../../Shaders/out/Test_shader.frag.spv", pipelineConfig);
	}

	void Engine::createCommandBuffers()
	{
		commandBuffers.resize(swapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device->getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device->device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers.");
		}

		for (int i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to begin recording command buffer.");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = swapChain->getRenderPass();
			renderPassInfo.framebuffer = swapChain->getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0, 0, 0, 0 };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			pipeline->bind(commandBuffers[i]);
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to record command buffer.");
			}
		}
	}

	void Engine::drawFrame()
	{
		uint32_t imageIndex;
		auto result = swapChain->acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			//throw std::runtime_error("Failed to acquire swap chain image.");
		}

		result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
		{
			//throw std::runtime_error("Failed to present swap chain image.");
		}
	}

	void Engine::recreateSwapChain()
	{

		int width = 0, height = 0;
		glfwGetFramebufferSize(_platform->GetWindow(), &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(_platform->GetWindow(), &width, &height);
			glfwWaitEvents();
		}

		cleanupSwapChain();

		vkDeviceWaitIdle(device->device());

		swapChain->createSwapChain();
		swapChain->createImageViews();
		swapChain->createRenderPass();

		auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(swapChain->width(), swapChain->height());
		pipelineConfig.renderPass = swapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		pipeline->createGraphicsPipeline("../../../Shaders/out/Test_shader.vert.spv", "../../../Shaders/out/Test_shader.frag.spv", pipelineConfig);
		swapChain->createFramebuffers();
		createCommandBuffers();
	}

	void Engine::cleanupSwapChain()
	{
		for (size_t i = 0; i < swapChain->getSwapChainFB().size(); i++) {
			vkDestroyFramebuffer(device->device(), swapChain->getSwapChainFB()[i], nullptr);
		}

		vkFreeCommandBuffers(device->device(), device->getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		vkDestroyPipeline(device->device(), pipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipelineLayout(device->device(), pipelineLayout, nullptr);
		vkDestroyRenderPass(device->device(), swapChain->getRenderPass(), nullptr);

		for (size_t i = 0; i < swapChain->getSwapChainImageViews().size(); i++) {
			vkDestroyImageView(device->device(), swapChain->getImageView(i), nullptr);
		}

		vkDestroySwapchainKHR(device->device(), swapChain->getSwapChain(), nullptr);
	}
}