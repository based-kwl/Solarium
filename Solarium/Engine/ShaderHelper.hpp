#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>
#include "Pipeline.hpp"
#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

namespace Solarium
{
	struct ShaderModules
	{
		vk::ShaderModule module;
		vk::ShaderStageFlagBits shaderType; // 0 = vertex; 1 = fragment
		ShaderModules(vk::ShaderModule mod, vk::ShaderStageFlagBits type)
		{
			this->module = mod;
			this->shaderType = type;
		}
	};

	struct ShaderLocs
	{
		std::string vertexShaderLoc;
		std::string fragmentShaderLoc;
		
		ShaderLocs(std::string vertexShader, std::string fragmentShader)
		{
			this->fragmentShaderLoc = fragmentShader;
			this->vertexShaderLoc = vertexShader;
		}
	};

	class ShaderHelper
	{
	public:
		ShaderHelper(const std::string& shadersPath, const PipelineConfigInfo& configInfo, vk::Device device);
		std::vector<ShaderModules> getShaderModules() { return shaderModules_; }

	private:

		std::vector<ShaderLocs> getShaderPaths(const std::string& shadersPath);
		std::string getSiblingShaderPath(const std::filesystem::path shaderPath);
		void compileShaders(std::vector<ShaderLocs> shaderLocs, vk::Device device);
		std::string readFile(const std::string& fileName);
		std::vector<ShaderModules> shaderModules_;
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages_;
	};
}