#include "VulkanHelper.hpp"
#undef NDEBUG
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

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) ? Solarium::Logger::Error("VALIDATION LAYER: ", pCallbackData->pMessage) : Solarium::Logger::Warn("VALIDATION LAYER: ", pCallbackData->pMessage);
    }
    else if (messageSeverity <= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) ? Solarium::Logger::Log("VALIDATION LAYER: ", pCallbackData->pMessage) : Solarium::Logger::Trace("VALIDATION LAYER: ", pCallbackData->pMessage);
    }
    Solarium::Logger::Log("VALIDATION LAYER: ");
    std::cout << "VALIDATION LAYER: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}


namespace Solarium
{

    void VulkanHelper::InitializeInstance(GLFWwindow* window)
    {
        vk::ApplicationInfo appInfo("Solarium", VK_MAKE_VERSION(1,0,0), "Solarium", VK_MAKE_VERSION(1,0,0), VK_API_VERSION_1_2);
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> glfwExtensionsVector(glfwExtensions, glfwExtensions + glfwExtensionCount);
        
        
        glfwExtensionsVector.push_back("VK_EXT_debug_utils");
        auto layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };

        auto instance = vk::createInstanceUnique(
        vk::InstanceCreateInfo{ {}, &appInfo, static_cast<uint32_t>(layers.size()), layers.data(),
            static_cast<uint32_t>(glfwExtensionsVector.size()), glfwExtensionsVector.data() });
        
        auto dldi = vk::DispatchLoaderDynamic(*instance, vkGetInstanceProcAddr);
        auto messenger = instance->createDebugUtilsMessengerEXTUnique(
        vk::DebugUtilsMessengerCreateInfoEXT{ {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            DebugCallback },
        nullptr, dldi);

    }

}