#include "Pipeline.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace Solarium
{

	Pipeline::Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo) : ldevice{ device }
	{
		createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
	}

	std::vector<char> Pipeline::readFile(const std::string& filepath)
	{
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

		if (!file.is_open())
		{
			std::cout << filepath << std::endl;
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<rsize_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	Pipeline::~Pipeline()
	{
		ldevice.device().destroyShaderModule(vertShaderModule);
		ldevice.device().destroyShaderModule(fragShaderModule);
		ldevice.device().destroyPipeline(graphicsPipeline);
	}

	void Pipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo)
	{
		//assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no pipelineLayout provided in configInfo");
		//assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no pipelineLayout provided in configInfo");

		auto vertCode = readFile(vertFilepath);
		auto fragCode = readFile(fragFilepath);

		createShaderModule(vertCode, &vertShaderModule);
		createShaderModule(fragCode, &fragShaderModule);

		vk::PipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].stage = vk::ShaderStageFlagBits::eVertex;
		shaderStages[0].module = vertShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = {};
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		shaderStages[1].sType = vk::StructureType::ePipelineShaderStageCreateInfo;
		shaderStages[1].stage = vk::ShaderStageFlagBits::eFragment;
		shaderStages[1].module = fragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = {};
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;

		vk::PipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &configInfo.viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &configInfo.scissor;

		vk::GraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = nullptr;

		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = nullptr;
		graphicsPipeline = ldevice.device().createGraphicsPipelines(nullptr,pipelineInfo).value[0];
		if (!graphicsPipeline) {
			throw std::runtime_error("Failed to create graphics pipeline.");
		}

	}

	void Pipeline::createShaderModule(const std::vector<char>& code, vk::ShaderModule* shaderModule)
	{
		vk::ShaderModuleCreateInfo createInfo{};
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		*shaderModule = ldevice.device().createShaderModule(createInfo);
		if (!shaderModule)
		{
			throw std::runtime_error("Failed to create shader module.");
		}
	}

	void Pipeline::bind(vk::CommandBuffer commandBuffer)
	{
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
	}

	PipelineConfigInfo Pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		PipelineConfigInfo configInfo{};

		configInfo.inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;

		configInfo.scissor.offset.x = 0;
		configInfo.scissor.offset.y = 0;
		
		configInfo.scissor.extent.width = width;
		configInfo.scissor.extent.height = height;

		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = vk::CullModeFlagBits::eNone;
		configInfo.rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
		configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		configInfo.colorBlendAttachment.colorWriteMask =
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;   // Optional
		configInfo.colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;  // Optional
		configInfo.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;              // Optional
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;   // Optional
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;  // Optional
		configInfo.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;              // Optional

		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = vk::LogicOp::eCopy;  // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;

		return configInfo;
	}

}