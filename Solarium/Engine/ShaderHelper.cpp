#include "ShaderHelper.hpp"

namespace Solarium
{
	ShaderHelper::ShaderHelper(const std::string& shadersPath, const PipelineConfigInfo& configInfo, vk::Device device)
	{
		std::vector<ShaderSet> shaders = getShaderPaths(shadersPath);
		auto thing = compileShaders(shaders, device);
	}

	std::vector<vk::ShaderModule> ShaderHelper::compileShaders(std::vector<ShaderSet> shaders, vk::Device device)
	{
		std::vector<vk::ShaderModule> shaderModules;

		for (auto& shaderSet : shaders)
		{
			std::string vertexShader = readFile(shaderSet.vertexShaderLoc);
			std::string fragmentShader = readFile(shaderSet.fragmentShaderLoc);
	
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
	
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
	
			shaderc::SpvCompilationResult vertexModule = compiler.CompileGlslToSpv(vertexShader, shaderc_shader_kind::shaderc_glsl_vertex_shader, shaderSet.vertexShaderLoc.c_str());
			shaderc::SpvCompilationResult fragmentModule = compiler.CompileGlslToSpv(fragmentShader, shaderc_shader_kind::shaderc_glsl_fragment_shader, shaderSet.fragmentShaderLoc.c_str());
			if (vertexModule.GetCompilationStatus() != shaderc_compilation_status_success) {
  				std::cerr << vertexModule.GetErrorMessage();
				break;
  			}
			if (fragmentModule.GetCompilationStatus() != shaderc_compilation_status_success) {
  				std::cerr << fragmentModule.GetErrorMessage();
				break;
  			}
			
			vk::ShaderModuleCreateInfo createInfo{};
			createInfo.codeSize = fragmentModule.begin() - fragmentModule.end();
			createInfo.pCode = fragmentModule.begin();

			shaderModules.push_back(device.createShaderModule(createInfo));
			if (shaderModules.front() == VK_NULL_HANDLE)
			{
				throw std::runtime_error("Failed to create shader module.");
			}

			createInfo.codeSize = vertexModule.begin() - vertexModule.end();
			createInfo.pCode = vertexModule.begin();

			shaderModules.push_back(device.createShaderModule(createInfo));
			if (shaderModules.front() == VK_NULL_HANDLE)
			{
				throw std::runtime_error("Failed to create shader module.");
			}
			
		}
		

		
	}

	std::string ShaderHelper::readFile(const std::string& fileName)
	{
		std::ifstream in(fileName, std::ios::in | std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		throw(errno);
	}
	std::string ShaderHelper::getSiblingShaderPath(const std::filesystem::path shaderPath)
	{
		if(shaderPath.extension() == ".frag") 
		{
			return shaderPath.parent_path().string() + '/' + shaderPath.stem().string() + (".vert"); 
		} 
		else 
		{
			return shaderPath.parent_path().string() + '/' + shaderPath.stem().string() + (".frag") ;
		}
	}

	std::vector<ShaderSet> ShaderHelper::getShaderPaths(const std::string& shadersPath)
	{
	    std::vector<ShaderSet> out;
		std::vector<std::string> ignoredShaders;
	    for(auto &p : std::filesystem::recursive_directory_iterator(shadersPath))
	    {
	        if(p.path().extension() == ".vert" && std::filesystem::exists(getSiblingShaderPath(p.path())) && !std::count(ignoredShaders.begin(), ignoredShaders.end(), p.path().stem().string())) 
	        {
	            out.push_back(ShaderSet((p.path().string(), getSiblingShaderPath(p.path()))));
				ignoredShaders.push_back(p.path().stem().string());
	        } else if(p.path().extension() == ".frag" && std::filesystem::exists(getSiblingShaderPath(p.path())) && !std::count(ignoredShaders.begin(), ignoredShaders.end(), p.path().stem().string()))
	        {
				out.push_back(ShaderSet((getSiblingShaderPath(p.path()), p.path().string()))); 
				ignoredShaders.push_back(p.path().stem().string());
	        }
	    }
	    return out;
	}
}

