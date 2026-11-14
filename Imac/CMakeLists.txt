cmake_minimum_required(VERSION 3.10)
project(iMac_3D_Model)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Libraries are in the same folder as the project
set(LIBRARIES_BASE "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries")

set(GLEW_INCLUDE_DIR "${LIBRARIES_BASE}/glew/include")
set(GLEW_LIBRARY "${LIBRARIES_BASE}/glew/lib/Release/x64/glew32.lib")
set(GLEW_DLL "${LIBRARIES_BASE}/glew/bin/Release/x64/glew32.dll")

set(GLFW_INCLUDE_DIR "${LIBRARIES_BASE}/glfw/include")
set(GLFW_LIBRARY "${LIBRARIES_BASE}/glfw/lib-vc2022/glfw3.lib")

set(GLM_INCLUDE_DIR "${LIBRARIES_BASE}/glm")

# Add executable
add_executable(Mac_Scene
    MainCode.cpp
    SceneManager.cpp
    ViewManager.cpp
    ShaderManager.cpp
    ShapeMeshes.cpp
)

# Include directories
target_include_directories(Mac_Scene PRIVATE
    ${GLEW_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(Mac_Scene
    ${GLEW_LIBRARY}
    ${GLFW_LIBRARY}
    opengl32
)

# Copy GLEW DLL to output directory after build
add_custom_command(TARGET Mac_Scene POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLEW_DLL}"
    $<TARGET_FILE_DIR:Mac_Scene>
)

# Print status messages to verify paths
message(STATUS "Libraries Base: ${LIBRARIES_BASE}")
message(STATUS "GLEW Include: ${GLEW_INCLUDE_DIR}")
message(STATUS "GLEW Library: ${GLEW_LIBRARY}")
message(STATUS "GLFW Include: ${GLFW_INCLUDE_DIR}")
message(STATUS "GLFW Library: ${GLFW_LIBRARY}")
message(STATUS "GLM Include: ${GLM_INCLUDE_DIR}")
