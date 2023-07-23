# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("androidvulkandemo")


find_package(Vulkan REQUIRED)

set(Link_Libraries "")
set(Include_Diirectories "")

if(ANDROID)
        find_library( # Sets the name of the path variable.
                log-lib
                # Specifies the name of the NDK library that
                # you want CMake to locate.
                log)


        list(APPEND Link_Libraries "${log-lib}")
        list(APPEND Include_Diirectories "${ANDROID_NDK}/sources/third_party/vulkan/src/include/")
else()
        message("Build for windows")
        find_package(glfw3 CONFIG REQUIRED)     
        find_package(imgui CONFIG REQUIRED)

        list(APPEND Link_Libraries "glfw")
        list(APPEND Link_Libraries "imgui::imgui")

endif()

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.


add_library(Renderer SHARED "Renderer.cpp" "Renderer.hpp")
target_compile_features(Renderer PRIVATE cxx_std_20)
target_include_directories(Renderer PRIVATE ${Include_Diirectories})   
target_link_libraries(Renderer PRIVATE Vulkan::Vulkan ${Link_Libraries})


if(NOT ANDROID)
        add_executable(RendererExecutable  "main.cpp")
        target_link_libraries(RendererExecutable  PRIVATE Renderer glfw imgui::imgui)
        target_compile_features(RendererExecutable  PRIVATE cxx_std_20)
endif()