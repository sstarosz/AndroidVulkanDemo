cmake_minimum_required(VERSION 3.24)

project(StMath
		VERSION 0.0.1
		DESCRIPTION "Library for mathematical oeprations"
		LANGUAGES CXX)

# Function to add parent directory of each header to target's include directories
function(add_parent_directory_for_headers TARGET_NAME HEADER_FILES)
    foreach(HEADER ${HEADER_FILES})
        get_filename_component(HEADER_DIR ${HEADER} DIRECTORY)
        target_include_directories(${TARGET_NAME} PRIVATE ${HEADER_DIR})
    endforeach()
endfunction()



set(Sources
	"Sources/Matrix4x4.cpp"
	"Sources/Vector2.cpp"
	"Sources/Vector3.cpp"
	"Sources/Vector4.cpp")

set(Private_Headers
	)

set(Public_Headers
	"Include/StMath/StMath.hpp"
	"Include/StMath/Matrix4x4.hpp"
	"Include/StMath/Vector2.hpp"
	"Include/StMath/Vector3.hpp"
	"Include/StMath/Vector4.hpp"
)


add_library(${PROJECT_NAME} ${Sources} ${Private_Headers} ${Public_Headers})

message("!!!!! ${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include/StMath") 
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Sources") 


#Common library feature
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE ${Compiler_Flags})




#generate_documentation(TargetName)
