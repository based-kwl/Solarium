# CMakeList.txt : CMake project for Solarium, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(Solarium)

include_directories(include)
link_directories(lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Add source to this project's executable.
add_executable (Solarium "Defines.hpp" "Typedef.h" "Engine/Solarium.cpp" "Engine/Solarium.hpp" "Engine/Logger.cpp" "Engine/Logger.hpp"  "Engine/Platform.cpp" "Engine/Platform.hpp"  "Engine/Engine.cpp" "Engine/Engine.hpp" "Engine/Device.hpp" "Engine/Device.cpp" "Engine/Pipeline.hpp" "Engine/Pipeline.cpp" "Engine/SwapChain.hpp" "Engine/SwapChain.cpp" "Engine/ShaderHelper.cpp" "Engine/ShaderHelper.hpp" "Engine/UBO.cpp" "Engine/UBO.hpp"  "Engine/BufferHelper.hpp" "Engine/BufferHelper.cpp" "Engine/Texture.cpp" "Engine/Texture.hpp")
target_link_libraries(Solarium vulkan-1 glfw3 shaderc_combined)

# TODO: Add tests and install targets if needed.
