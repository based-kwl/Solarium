#include "ShaderHelper.hpp"

namespace Solarium
{

	std::string replaceString(std::string subject, const std::string& search,
	                          const std::string& replace) {
	    size_t pos = 0;
	    while((pos = subject.find(search, pos)) != std::string::npos) {
	         subject.replace(pos, search.length(), replace);
	         pos += replace.length();
	    }
	    return subject;
	}

	ShaderHelper::ShaderHelper(const std::string& shadersPath, const PipelineConfigInfo& configInfo, vk::Device device)
	{
		std::vector<ShaderLocs> shaders = getShaderPaths(shadersPath);
		compileShaders(shaders, device);
	}

	void ShaderHelper::compileShaders(std::vector<ShaderLocs> shaderLocs, vk::Device device)
	{
		std::vector<ShaderModules> shaderModules;

		for (auto& shaderLoc : shaderLocs)
		{
			shaderLoc.fragmentShaderLoc = replaceString(shaderLoc.fragmentShaderLoc, "\\", "/");
			shaderLoc.vertexShaderLoc = replaceString(shaderLoc.vertexShaderLoc, "\\", "/");
			std::string vertexShader = readFile(shaderLoc.vertexShaderLoc);
			std::string fragmentShader = readFile(shaderLoc.fragmentShaderLoc);
	
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
	
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
	
			shaderc::SpvCompilationResult vertexModule = compiler.CompileGlslToSpv(vertexShader, shaderc_shader_kind::shaderc_glsl_vertex_shader, shaderLoc.vertexShaderLoc.c_str());
			shaderc::SpvCompilationResult fragmentModule = compiler.CompileGlslToSpv(fragmentShader, shaderc_shader_kind::shaderc_glsl_fragment_shader, shaderLoc.fragmentShaderLoc.c_str());
			if (vertexModule.GetCompilationStatus() != shaderc_compilation_status_success) {
  				std::cerr << vertexModule.GetErrorMessage();
				break;
  			}
			if (fragmentModule.GetCompilationStatus() != shaderc_compilation_status_success) {
  				std::cerr << fragmentModule.GetErrorMessage();
				break;
  			}
			
			vk::ShaderModuleCreateInfo createInfo{};
			createInfo.codeSize = fragmentModule.length();
			createInfo.pCode = fragmentModule.begin();

			shaderModules.push_back(ShaderModules(device.createShaderModule(createInfo), vk::ShaderStageFlagBits::eFragment));
			if (shaderModules.front().module == VK_NULL_HANDLE)
			{
				throw std::runtime_error("Failed to create shader module.");
			}

			createInfo.codeSize = vertexModule.length();
			createInfo.pCode = vertexModule.begin();

			shaderModules.push_back(ShaderModules(device.createShaderModule(createInfo), vk::ShaderStageFlagBits::eVertex));
			if (shaderModules.front().module == VK_NULL_HANDLE)
			{
				throw std::runtime_error("Failed to create shader module.");
			}
		}
		shaderModules_ = shaderModules;
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

	std::vector<ShaderLocs> ShaderHelper::getShaderPaths(const std::string& shadersPath)
	{
	    std::vector<ShaderLocs> out;
		std::vector<std::string> ignoredShaders;
	    for(auto &p : std::filesystem::recursive_directory_iterator(shadersPath))
	    {
	        if(p.path().extension() == ".vert" && std::filesystem::exists(getSiblingShaderPath(p.path())) && !std::count(ignoredShaders.begin(), ignoredShaders.end(), p.path().stem().string())) 
	        {
	            out.push_back(ShaderLocs(p.path().string(), getSiblingShaderPath(p.path())));
				ignoredShaders.push_back(p.path().stem().string());
	        } else if(p.path().extension() == ".frag" && std::filesystem::exists(getSiblingShaderPath(p.path())) && !std::count(ignoredShaders.begin(), ignoredShaders.end(), p.path().stem().string()))
	        {
				out.push_back(ShaderLocs(getSiblingShaderPath(p.path()), p.path().string())); 
				ignoredShaders.push_back(p.path().stem().string());
	        }
	    }
		shaderModules_.reserve(out.size() * 2);
	    return out;
	}
}

