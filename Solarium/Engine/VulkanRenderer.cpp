#include "VulkanRenderer.hpp"
#include "VulkanHelper.hpp"

namespace Solarium
{
    VulkanRenderer::VulkanRenderer(Platform* platform)
    {
        GLFWwindow* window = platform->GetWindow();
        VulkanHelper::InitializeInstance(window);
        
    }
	VulkanRenderer::~VulkanRenderer()
    {

    }

}