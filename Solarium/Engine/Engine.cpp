#include "Engine.hpp"

namespace Solarium
{
	Engine::Engine(const char* applicationName)
	{
		Solarium::Logger::Log("INITIALIZING");
		_platform = new Platform(applicationName);
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
	}

	void Engine::OnLoop(const uint32_t deltaTime)
	{

	}

	void Engine::createPipelineLayout()
	{
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		pipelineLayout = device->device().createPipelineLayout(pipelineLayoutInfo);
		if (!pipelineLayout)
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

		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandPool = device->getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		commandBuffers = device->device().allocateCommandBuffers(allocInfo);
		if (commandBuffers.empty())
		{
			throw std::runtime_error("Failed to allocate command buffers.");
		}

		for (int i = 0; i < commandBuffers.size(); i++)
		{
			vk::CommandBufferBeginInfo beginInfo{};
			commandBuffers[i].begin(beginInfo);

			vk::RenderPassBeginInfo renderPassInfo{};
			renderPassInfo.renderPass = swapChain->getRenderPass();
			renderPassInfo.framebuffer = swapChain->getFrameBuffer(i);

			renderPassInfo.renderArea.offset.x = 0;
			renderPassInfo.renderArea.offset.y = 0;
			
			renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

			std::array<vk::ClearValue, 2> clearValues{};
			clearValues[0].color.setFloat32({0.1,0.1,0.1,0.1});
			clearValues[1].setDepthStencil({ 1.0f, 0 });
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
			commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
			pipeline->bind(commandBuffers[i]);
			commandBuffers[i].draw(3,1,0,0);

			commandBuffers[i].endRenderPass();
			commandBuffers[i].end();
		}
	}
	void Engine::drawFrame()
	{
		uint32_t imageIndex;
		auto result = swapChain->acquireNextImage(&imageIndex);

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image.");
		}

		result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to present swap chain image.");
		}
	}
}