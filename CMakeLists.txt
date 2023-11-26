cmake_minimum_required(VERSION 3.22.1)

project("androidvulkandemo"
        VERSION 0.1
        DESCRIPTION "Android Vulkan Demo"
        LANGUAGES CXX
        )


find_package(Vulkan REQUIRED)
find_package(Stb REQUIRED)

if(NOT ANDROID)
    find_package(imgui CONFIG REQUIRED)
endif()



add_subdirectory(Assets)
add_subdirectory(Renderer)

add_subdirectory(Samples)