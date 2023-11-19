cmake_minimum_required(VERSION 3.22.1)

project("androidvulkandemo")

find_package(Vulkan REQUIRED)
find_package(Stb REQUIRED)
find_package(imgui CONFIG REQUIRED)



add_subdirectory(Assets)
add_subdirectory(Renderer)

add_subdirectory(Samples)