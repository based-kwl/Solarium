# CMakeList.txt : CMake project for Solarium, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(include)
link_directories(lib)

# Add source to this project's executable.
add_executable (Solarium "Engine/Solarium.cpp" "Engine/Solarium.h" "Engine/Logger.h" "Engine/Logger.cpp" "Engine/Platform.h" "Engine/Engine.h" "Engine/Engine.cpp" "Engine/Platform.cpp")
target_link_libraries(Solarium vulkan-1 glfw3)

# TODO: Add tests and install targets if needed.
