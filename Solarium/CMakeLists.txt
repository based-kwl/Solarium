# CMakeList.txt : CMake project for Solarium, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(include)
link_directories(lib)

# Add source to this project's executable.
add_executable (Solarium "Defines.hpp" "Typedef.h" "Engine/Solarium.cpp" "Engine/Solarium.hpp" "Engine/Logger.cpp" "Engine/Logger.hpp"  "Engine/Platform.cpp" "Engine/Platform.hpp"  "Engine/Engine.cpp" "Engine/Engine.hpp" "Engine/VulkanRenderer.cpp" "Engine/VulkanRenderer.hpp" "Engine/VulkanHelper.cpp" "Engine/VulkanHelper.hpp" "Engine/Device.hpp" "Engine/Device.cpp" "Engine/Pipeline.hpp" "Engine/Pipeline.cpp")
target_link_libraries(Solarium vulkan-1 glfw3)

# TODO: Add tests and install targets if needed.
