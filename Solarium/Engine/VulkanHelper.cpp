
#include "VulkanHelper.hpp"
#ifndef NDEBUG
#define VK_CHECK_RESULT(f)																				\
{																										\
	vk::Result res = (f);																					\
	if (res != vk::Result::eSuccess)																				\
	{																									\
		std::cout << "Fatal : vk::Result is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
		assert(res == vk::Result::eSuccess);																		\
	}																									\
}
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
namespace Solarium
{

    void VulkanHelper::InitializeInstance(GLFWwindow* window, vk::Instance instance)
    { 
        vk::DynamicLoader dl;
        vk::ApplicationInfo appInfo("Solarium", VK_MAKE_VERSION(1,0,0), "Solarium", VK_MAKE_VERSION(1,0,0), VK_API_VERSION_1_2);
        
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> glfwExtensionsVector(glfwExtensions, glfwExtensions + glfwExtensionCount);
        glfwExtensionsVector.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        auto layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        instance = vk::createInstance(
        vk::InstanceCreateInfo{ {}, &appInfo, static_cast<uint32_t>(layers.size()), layers.data(),
            static_cast<uint32_t>(glfwExtensionsVector.size()), glfwExtensionsVector.data() });
        
        VULKAN_HPP_DEFAULT_DISPATCHER.init( instance );
        auto messenger = instance.createDebugUtilsMessengerEXT(
        vk::DebugUtilsMessengerCreateInfoEXT{ {},
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            debugCallback },
        nullptr);
    }

}