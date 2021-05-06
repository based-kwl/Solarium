#include "Engine.hpp"
namespace Solarium
{

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
		uniformBufferObject = new UBO(swapChain, device);
		texture = new Texture(swapChain, device);
		vertexBuffer = new VertexBuffer(device);
		texture->createImageViews();
		createPipelineLayout();
		createPipeline();
		texture->createChain();
		vertexBuffer->createChain();
		uniformBufferObject->createChain(texture->getTextureSampler(), texture->getTextureImageView());
		createCommandBuffers();
	}

	Engine::~Engine()
	{
		cleanupSwapChain();

		device->device().destroySampler(texture->getTextureSampler());
		device->device().destroyImageView(texture->getTextureImageView());
		device->device().destroyImage(texture->getTextureImage());
		device->device().freeMemory(texture->getTextureImageMemory());
		device->device().destroyDescriptorSetLayout(uniformBufferObject->getDescriptorSetLayout());
		device->device().destroyBuffer(vertexBuffer->getIndexBuffer());
		device->device().freeMemory(texture->getIndexBufferMemory());
		device->device().destroyBuffer(vertexBuffer->getVertexBuffer());
		device->device().freeMemory(vertexBuffer->getVertexBufferMemory());
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


	float Engine::getdt()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	}

	void Engine::createPipelineLayout()
	{
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		vk::DescriptorSetLayout descriptorSetLayout = uniformBufferObject->getDescriptorSetLayout();
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayout = device->device().createPipelineLayout(pipelineLayoutInfo, nullptr);
		if (!pipelineLayout)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void Engine::createPipeline()
	{
		auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(swapChain->width(), swapChain->height());
		pipelineConfig.renderPass = swapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipeline = new Pipeline(*device, "../../../Shaders/out/Test_shader.vert.spv", "../../../Shaders/out/Test_shader.frag.spv", pipelineConfig);
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
			std::vector<vk::Buffer> vertexBuffers = { vertexBuffer->getVertexBuffer()};
			std::vector<vk::DeviceSize> offsets = {0};
			commandBuffers[i].bindVertexBuffers(0, vertexBuffers, offsets);
			commandBuffers[i].bindIndexBuffer(vertexBuffer->getIndexBuffer(), 0, vk::IndexType::eUint16);
			commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, uniformBufferObject->getDescriptorSets()[i], nullptr);
			commandBuffers[i].drawIndexed(static_cast<uint32_t>(vertexBuffer->indices.size()), 1, 0, 0, 0);
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
		updateUniformBuffers(imageIndex);

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

	void Engine::updateUniformBuffers(uint32_t imageIndex)
	{
		ubos.viewmodel.model = glm::rotate(glm::mat4(1.0f), Engine::getdt() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubos.viewmodel.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubos.viewmodel.proj = glm::perspective(glm::radians(45.0f), swapChain->width() / (float)swapChain->height(), 0.1f, 10.0f);
		ubos.viewmodel.proj[1][1] *= -1;
		ubos.viewmodel.rotation = glm::vec3();
		ubos.viewmodel.position = glm::vec3();
		ubos.viewmodel.viewPos = glm::vec4();
		uniformBufferObject->updateUniformbuffer(imageIndex, UBOType::VIEWMODEL, ubos);
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
		updateAll();
		createPipelineLayout();
		createPipeline();
		texture->createChain();
		vertexBuffer->createChain();
		uniformBufferObject->createChain(texture->getTextureSampler(), texture->getTextureImageView());
		createCommandBuffers();

		swapChain->getImagesInFlight().resize(swapChain->imageCount());
	}

	void Engine::updateAll() {
		texture->update(swapChain, device);
		vertexBuffer->update(swapChain, device);
		uniformBufferObject->update(swapChain, device);
	}

	void Engine::cleanupSwapChain()
	{
		for (size_t i = 0; i < swapChain->getSwapChainFB().size(); i++)
		{
			device->device().destroyFramebuffer(swapChain->getSwapChainFB()[i]);
		}

		for (size_t i = 0; i < swapChain->imageCount(); i++)
		{
			device->device().destroyBuffer(uniformBufferObject->getUniformBuffers(UBOType::VIEWMODEL)[i]);
			device->device().freeMemory(uniformBufferObject->getUniformBuffersMemory(UBOType::VIEWMODEL)[i]);
			device->device().destroyBuffer(uniformBufferObject->getUniformBuffers(UBOType::CAMERA)[i]);
			device->device().freeMemory(uniformBufferObject->getUniformBuffersMemory(UBOType::CAMERA)[i]);
		}

		device->device().destroyDescriptorPool(uniformBufferObject->getDescriptorPool());

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
		device->device().destroyDescriptorSetLayout(uniformBufferObject->getDescriptorSetLayout(), nullptr);
		glfwDestroyWindow(_platform->GetWindow());

		glfwTerminate();
	}

}
