#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "Pipeline.hpp"
#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

namespace Solarium
{
	struct ShaderSet
	{
		std::string vertexShaderLoc;
		std::string fragmentShaderLoc;
		//ShaderSet(std::string vertexShader, std::string fragmentShader);
	};
	class ShaderHelper
	{
	public:
		ShaderHelper(const std::string& shadersPath, const PipelineConfigInfo& configInfo, vk::Device device);
		//vk::GraphicsPipelineCreateInfo getInfo();
		std::vector<ShaderSet> getShaderPaths(const std::string& shadersPath);
	private:
		std::string getSiblingShaderPath(const std::filesystem::path shaderPath);
		std::vector<vk::ShaderModule> compileShaders(std::vector<ShaderSet> shaders, vk::Device device);
		std::string readFile(const std::string& fileName);
		//vk::GraphicsPipelineCreateInfo getShader();
	};


}