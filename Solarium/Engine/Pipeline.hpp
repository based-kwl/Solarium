#pragma once

#include "Device.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Solarium
{
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;
		static vk::VertexInputBindingDescription getBindingDescription() {
			vk::VertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = vk::VertexInputRate::eVertex;
			return bindingDescription;
		}
		static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			std::array<vk::VertexInputAttributeDescription, 2> attributeDescription{};

			attributeDescription[0].binding = 0;
			attributeDescription[0].location = 0;
			attributeDescription[0].format = vk::Format::eR32G32Sfloat;
			attributeDescription[0].offset = offsetof(Vertex, pos);

			attributeDescription[1].binding = 0;
			attributeDescription[1].location = 1;
			attributeDescription[1].format = vk::Format::eR32G32B32Sfloat;
			attributeDescription[1].offset = offsetof(Vertex, color);
			 
			return attributeDescription;
		}
	};

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