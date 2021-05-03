#include "Engine.hpp"
namespace Solarium
{
	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},
		{ {0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}},
		{  {0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}},
		{ {-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
		app->setFramebufferResized(true);
	}
	
	Engine::Engine(const char* applicationName, uint32_t width, uint32_t height)
	{
		Solarium::Logger::Log("INITIALIZING");
		_platform = new Platform(applicationName, width, height);
		glfwSetFramebufferSizeCallback(_platform->GetWindow(), framebufferResizeCallback);
		
		device = new Device{ *_platform };
		swapChain = new SwapChain(*device, _platform->getExtent());
		createPipelineLayout();
		createPipeline();
		createVertexBuffer();
		createIndexBuffer();
		createCommandBuffers();
	}

	Engine::~Engine()
	{
		cleanupSwapChain();

		device->device().destroyBuffer(indexBuffer);
		device->device().freeMemory(indexBufferMemory);
		device->device().destroyBuffer(vertexBuffer);
		device->device().freeMemory(vertexBufferMemory);
	}

	void Engine::Run()
	{
		while (!glfwWindowShouldClose(_platform->GetWindow()))
		{
			glfwPollEvents();
			try {
				drawFrame();
			}
			catch (vk::OutOfDateKHRError outOfDateKHRError)
			{
			}
		}
		device->device().waitIdle();
	}

	void Engine::OnLoop(const uint32_t deltaTime)
	{

	}

	uint32_t Engine::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		vk::PhysicalDeviceMemoryProperties memProperties = device->physicalDevice().getMemoryProperties();

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		throw std::runtime_error("Failed to find suitable memory type");
	}

	void Engine::createBuffer(
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

		buffer = device->device().createBuffer(bufferInfo);

		if (!buffer)
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}

		vk::MemoryRequirements memRequirements = device->device().getBufferMemoryRequirements(buffer);

		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		bufferMemory = device->device().allocateMemory(allocInfo);

		if (VK_NULL_HANDLE)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		device->device().bindBufferMemory(buffer, bufferMemory, 0);
	}

	void Engine::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
	{
		vk::CommandBufferAllocateInfo allocInfo{ device->getCommandPool(), vk::CommandBufferLevel::ePrimary, 1 };
		vk::CommandBuffer commandBuffer;
		commandBuffer = device->device().allocateCommandBuffers(allocInfo)[0];

		vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
		commandBuffer.begin(beginInfo);
		vk::BufferCopy copyRegion{ 0,0,size };
		commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
		commandBuffer.end();

		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		device->graphicsQueue().submit(submitInfo);
		device->graphicsQueue().waitIdle();
		device->device().freeCommandBuffers(device->getCommandPool(), commandBuffer);

	}

	void Engine::createPipelineLayout()
	{
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ {}, 0, nullptr, 0 };
		pipelineLayout = device->device().createPipelineLayout(pipelineLayoutInfo, nullptr);
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
		pipeline = new Pipeline(*device, "../../../Shaders/out/Test_shader.vert.spv", "../../../Shaders/out/Test_shader.frag.spv", pipelineConfig);
	}

	void Engine::createVertexBuffer()
	{
		vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

		void* data = device->device().mapMemory(stagingBufferMemory, 0, bufferSize, {});
		memcpy(data, vertices.data(), (size_t)bufferSize);
		device->device().unmapMemory(stagingBufferMemory);

		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);
		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
		device->device().destroyBuffer(stagingBuffer);
		device->device().freeMemory(stagingBufferMemory);
	}

	void Engine::createIndexBuffer() {
		vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

		void* data = device->device().mapMemory(stagingBufferMemory, 0, bufferSize);
		memcpy(data, indices.data(), (size_t)bufferSize);
		device->device().unmapMemory(stagingBufferMemory);

		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

		copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		device->device().destroyBuffer(stagingBuffer);
		device->device().freeMemory(stagingBufferMemory);
	}

	void Engine::createCommandBuffers()
	{
		commandBuffers.resize(swapChain->imageCount());

		vk::CommandBufferAllocateInfo allocInfo{device->getCommandPool(), vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(commandBuffers.size())};

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

			renderPassInfo.renderArea = { { 0, 0 }, {swapChain->getSwapChainExtent()} };

			std::array<vk::ClearValue, 2> clearValues{};
			clearValues[0].color.setFloat32({ 0, 0, 0, 0 });
			clearValues[1].depthStencil.depth = 1.0f;
			clearValues[1].depthStencil.stencil = 0;

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

			pipeline->bind(commandBuffers[i]);
			std::vector<vk::Buffer> vertexBuffers = {vertexBuffer};
			std::vector<vk::DeviceSize> offsets = {0};
			commandBuffers[i].bindVertexBuffers(0, vertexBuffers, offsets);
			commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);
			commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
			commandBuffers[i].endRenderPass();
			commandBuffers[i].end();
		}
	}

	void Engine::drawFrame()
	{
		uint32_t imageIndex;
		std::vector<vk::Fence> images = swapChain->getImagesInFlight();
		std::vector<vk::Fence> fences = swapChain->getInFlightFences();
		size_t currentFrame = swapChain->getCurrentFrame();
		//auto result = swapChain->acquireNextImage(&imageIndex);
		vk::Result result = device->device().acquireNextImageKHR(swapChain->getSwapChain(), UINT64_MAX, (swapChain->getImageSemaphores())[currentFrame], {}, &imageIndex);
		if (result == vk::Result::eErrorOutOfDateKHR) {
			Logger::Log("EEEE");
			recreateSwapChain();
			return;
		}
		else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		if (images[imageIndex]) {
			device->device().waitForFences(images[imageIndex], VK_TRUE, UINT64_MAX);
		}
		swapChain->setImageInFlight(imageIndex, fences[currentFrame]);

		vk::SubmitInfo submitInfo{};

		vk::Semaphore waitSemaphores[] = { (swapChain->getImageSemaphores())[currentFrame] };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

		vk::Semaphore signalSemaphores[] = { (swapChain->getFinishedSemaphores())[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		device->device().resetFences(fences[currentFrame]);

		device->graphicsQueue().submit(submitInfo, fences[currentFrame]);

		vk::PresentInfoKHR presentInfo{};

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		vk::SwapchainKHR swapChains[] = { swapChain->getSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		result = device->presentQueue().presentKHR(presentInfo);

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized) {
			Logger::Log("EEEE");
			framebufferResized = false;
			recreateSwapChain();
		}
		else if (result != vk::Result::eSuccess) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		currentFrame = (currentFrame + 1) % swapChain->MAX_FRAMES_IN_FLIGHT;

	}

	void Engine::recreateSwapChain()
	{
		Solarium::Logger::Log("Resizing");
		int width = 0, height = 0;
		glfwGetFramebufferSize(_platform->GetWindow(), &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(_platform->GetWindow(), &width, &height);
			glfwWaitEvents();
		}

		device->device().waitIdle();

		cleanupSwapChain();

		device = new Device{ *_platform };
		swapChain = new SwapChain(*device, _platform->getExtent());
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();

		swapChain->getImagesInFlight().resize(swapChain->imageCount());
	}

	void Engine::cleanupSwapChain()
	{
		for (size_t i = 0; i < swapChain->getSwapChainFB().size(); i++)
		{
			device->device().destroyFramebuffer(swapChain->getSwapChainFB()[i]);
		}

		device->device().freeCommandBuffers(device->getCommandPool(), commandBuffers);

		device->device().destroyPipeline(pipeline->getGraphicsPipeline());
		device->device().destroyPipelineLayout(pipelineLayout);

		device->device().destroyRenderPass(swapChain->getRenderPass());
		
		for (int i = 0; i < swapChain->getSwapChainImageViews().size(); i++)
		{
			device->device().destroyImageView(swapChain->getImageView(i));
		}

		device->device().destroySwapchainKHR(swapChain->getSwapChain());
	}

	void Engine::cleanup() {
		cleanupSwapChain();

		for (size_t i = 0; i < swapChain->MAX_FRAMES_IN_FLIGHT; i++) 
		{
			device->device().destroySemaphore(swapChain->getFinishedSemaphores()[i]);
			device->device().destroySemaphore(swapChain->getImageSemaphores()[i]);
			device->device().destroyFence(swapChain->getInFlightFences()[i]);
		}

		device->device().destroyCommandPool(device->getCommandPool());
		device->device().destroy();
		device->getInstance().destroySurfaceKHR(device->surface());
		device->getInstance().destroy();

		glfwDestroyWindow(_platform->GetWindow());

		glfwTerminate();
	}

}
