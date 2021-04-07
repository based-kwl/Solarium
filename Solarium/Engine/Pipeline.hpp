#pragma once

#include "Device.hpp"
#include <string>
#include <vector>

namespace Solarium
{

	struct PipelineConfigInfo {
		vk::Viewport viewport;
		vk::Rect2D scissor;
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
		vk::PipelineMultisampleStateCreateInfo multisampleInfo;
		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
		vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
		vk::PipelineLayout pipelineLayout = nullptr;
		vk::RenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline
	{
	public:
		Pipeline(
			Device& device,
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);

		~Pipeline();
		Pipeline(const Pipeline&) = delete;
		void operator=(const Pipeline&) = delete;
		
		void bind(vk::CommandBuffer commandBuffer);

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
		void createGraphicsPipeline(
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);

		VkPipeline getGraphicsPipeline() { return graphicsPipeline; }

	private:
		static std::vector<char> readFile(const std::string& filepath);
		
		void createShaderModule(const std::vector<char>& code, vk::ShaderModule* shaderModule);

		Device& ldevice;
		vk::Pipeline graphicsPipeline;
		vk::ShaderModule vertShaderModule;
		vk::ShaderModule fragShaderModule;

	};
}