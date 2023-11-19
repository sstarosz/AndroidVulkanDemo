cmake_minimum_required(VERSION 3.24)

project(StMath
		VERSION 0.0.1
		DESCRIPTION "Library for mathematical oeprations"
		LANGUAGES CXX)


set(Sources
	"Matrix4x4.cpp"
	"Vector2.cpp"
	"Vector3.cpp"
	"Vector4.cpp")

set(Private_Headers
	)

set(Public_Headers
	"${CMAKE_SOURCE_DIR}/Renderer/Include/${PROJECT_NAME}/StMath.hpp"
	"${CMAKE_SOURCE_DIR}/Renderer/Include/${PROJECT_NAME}/Matrix4x4.hpp"
	"${CMAKE_SOURCE_DIR}/Renderer/Include/${PROJECT_NAME}/Vector2.hpp"
	"${CMAKE_SOURCE_DIR}/Renderer/Include/${PROJECT_NAME}/Vector3.hpp"
	"${CMAKE_SOURCE_DIR}/Renderer/Include/${PROJECT_NAME}/Vector4.hpp"
)


add_library(${PROJECT_NAME} ${Sources} ${Private_Headers} ${Public_Headers})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE ${Compiler_Flags})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Renderer/Include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Renderer/Include/${PROJECT_NAME}") 
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Renderer/Source/${PROJECT_NAME}") 
#generate_documentation(TargetName)




#generate_documentation(TargetName)
