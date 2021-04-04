# CMakeList.txt : CMake project for Solarium, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(include)
link_directories(lib)

# Add source to this project's executable.
add_executable (Solarium "Engine/Solarium.cpp" "Engine/Solarium.hpp" "Engine/Logger.hpp" "Engine/Logger.cpp" "Engine/Platform.hpp"  "Engine/Engine.cpp" "Engine/Platform.cpp" "Engine/VulkanRenderer.hpp")
target_link_libraries(Solarium vulkan-1 glfw3)

# TODO: Add tests and install targets if needed.
