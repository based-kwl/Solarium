#include <string>
#include <fstream>
#include <iostream>
#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

namespace Solarium
{
	class ShaderHelper
	{
	public:
        
        ShaderHelper(const char* vertexPath, const char* fragmentPath);
        vk::ShaderModule compileShader(std::string shaderSource, vk::ShaderModule* shaderModule, bool optimize = true);
    private:
        std::vector<char> readFile(const std::string &fileName);
	};


}