cmake_minimum_required(VERSION 3.24)

project(StRenderer
		VERSION 0.0.1
		DESCRIPTION "Vulkan Renderer"
		LANGUAGES CXX)


set(Sources
	"Camera.cpp"
	"Renderer.cpp")

set(Private_Headers
	)

set(Public_Headers
	"${CMAKE_SOURCE_DIR}/Renderer/Include/StRenderer/Camera.hpp"
	"${CMAKE_SOURCE_DIR}/Renderer/Include/StRenderer/Renderer.hpp")


add_library(${PROJECT_NAME} ${Sources} ${Private_Headers} ${Public_Headers})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE ${Compiler_Flags})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Renderer/Include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Renderer/Include/StRenderer") 
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Renderer/Source/StRenderer") 
target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

if(ANDROID)
	target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_ANDROID_NDK}/sources/third_party/vulkan/src/include)
endif()


target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
#generate_documentation(TargetName)
