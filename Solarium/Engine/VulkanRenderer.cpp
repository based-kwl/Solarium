#include "VulkanRenderer.hpp"
#include "VulkanHelper.hpp"

namespace Solarium
{
    VulkanRenderer::VulkanRenderer(Platform* platform, vk::Instance instance)
    {
        GLFWwindow* window = platform->GetWindow();
        VulkanHelper::InitializeInstance(window, instance);
        
    }
	VulkanRenderer::~VulkanRenderer()
    {

    }

}