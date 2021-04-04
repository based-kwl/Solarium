#include "VulkanHelper.hpp"

namespace Solarium
{

    void InitializeInstance()
    {
        vk::ApplicationInfo appInfo("Solarium", VK_MAKE_VERSION(1,0,0), "Solarium", VK_MAKE_VERSION(1,0,0), VK_API_VERSION_1_2);
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> glfwExtensionsVector(glfwExtensions, glfwExtensions + glfwExtensionCount);
        #ifdef NDEBUG
        glfwExtensionsVector.push_back("VK_EXT_debug_utils");
        std::vector<const char*> layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };
        #endif



    }

}