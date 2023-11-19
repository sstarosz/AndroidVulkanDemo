# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("androidvulkandemo")

project("Imgui_Renderer"
         VERSION 0.1.0
         DESCRIPTION "project-description-string"
         LANGUAGES CXX)


         



set(Sources
    "main.cpp")


add_executable(${PROJECT_NAME} ${Sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan StMath StRenderer StShader imgui::imgui)

add_dependencies(Imgui_Renderer Copy_Assets_File)
