#include <Shaders.hpp>
namespace Solarium
{
	class ShaderHelper
	{
		ShaderHelper(const char* vertexPath, const char* fragmentPath)
		{
			compileShader(readFile(vertexPath), readFile(fragmentPath));
		}
		
		std::string readFile(const std::string &fileName)
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
		vk::ShaderModule compileShader(std::string shaderSource, vk::ShaderModule* shaderModule, vk::Device device, bool optimize = true)
		{
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);
			shaderc::SpvCompilationResult compResult = compiler.CompileGlslToSpv(shaderSource, shaderc_glsl_infer_from_source, "shader_src", options);
			
			if (compResult.GetCompilationStatus() != shaderc_compilation_status_success) {
 				std::cerr << compResult.GetErrorMessage();
 				return nullptr;
 			} 

			vk::ShaderModuleCreateInfo createInfo{};
			createInfo.codeSize = compResult.cbegin() - compResult.cend();
			createInfo.pCode = compResult.cbegin();

			*shaderModule = device.createShaderModule(createInfo);
			if (!shaderModule)
			{
				throw std::runtime_error("Failed to create shader module.");
			}
		}
    };
}