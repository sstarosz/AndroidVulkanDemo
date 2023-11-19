 
 
 
add_custom_target(Copy_Assets_File 
                    DEPENDS ${CMAKE_BINARY_DIR}/Assets)
 
#TODO Copy Folder or create link to folders instahead of copy

add_custom_command(TARGET Copy_Assets_File POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy
                             ${CMAKE_SOURCE_DIR}/Assets/Models/Cube.obj
                             ${CMAKE_BINARY_DIR}/Assets/Models/Cube.obj
                     COMMENT "Copy Models"
                     )

add_custom_command(TARGET Copy_Assets_File POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy
                             ${CMAKE_SOURCE_DIR}/Assets/Models/Sphere.obj
                             ${CMAKE_BINARY_DIR}/Assets/Models/Sphere.obj
                     COMMENT "Copy Models"
                     )


add_custom_command(TARGET Copy_Assets_File POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_SOURCE_DIR}/Assets/Shaders/frag.spv
                        ${CMAKE_BINARY_DIR}/Assets/Shaders/frag.spv
                COMMENT "Copy fragment shader"
                )

add_custom_command(TARGET Copy_Assets_File POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_SOURCE_DIR}/Assets/Shaders/vert.spv
                        ${CMAKE_BINARY_DIR}/Assets/Shaders/vert.spv
                COMMENT "Copy vertex shader"
                )

add_custom_command(TARGET Copy_Assets_File POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_SOURCE_DIR}/Assets/Shaders/line_frag.spv
                        ${CMAKE_BINARY_DIR}/Assets/Shaders/line_frag.spv
                COMMENT "Copy line fragment shader"
                )

add_custom_command(TARGET Copy_Assets_File POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_SOURCE_DIR}/Assets/Shaders/line_vert.spv
                        ${CMAKE_BINARY_DIR}/Assets/Shaders/line_vert.spv
                COMMENT "Copy line vertex shader"
                )

add_custom_command(TARGET Copy_Assets_File POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_SOURCE_DIR}/Assets/Textures/texture.jpg
                        ${CMAKE_BINARY_DIR}/Assets/Textures/texture.jpg
                COMMENT "Copy texture"
                )

add_custom_command(TARGET Copy_Assets_File POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_SOURCE_DIR}/Assets/Textures/texture2.jpg
                        ${CMAKE_BINARY_DIR}/Assets/Textures/texture2.jpg
                COMMENT "Copy texture"
                )