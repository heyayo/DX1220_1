project(DX1220 VERSION 1.0.0)

file(GLOB SOURCE_FILES *.cpp)

if (UNIX)
set(CMAKE_EXE_LINKER_FLAGS "-lGL -lGLU -lGLEW -lglfw")
endif ()

add_executable(
        AIEXEC
        ${SOURCE_FILES}
        Entities/entity.cpp
        GridSystem.cpp
        GridSystem.hpp
        SceneA1TakeTwo.cpp
        SceneA1TakeTwo.hpp
        FSMs/statemachine.cpp FSMs/statemachine.hpp FSMs/state.cpp FSMs/state.hpp FSMs/birdai.cpp FSMs/birdai.hpp messager.cpp messager.hpp message.cpp message.hpp FSMs/bunnyai.cpp FSMs/bunnyai.hpp)

target_include_directories(AIEXEC PUBLIC ${CMAKE_SOURCE_DIR}/Common/)
target_include_directories(AIEXEC PUBLIC ${CMAKE_SOURCE_DIR}/include/)
target_include_directories(AIEXEC PUBLIC ${CMAKE_SOURCE_DIR}/AI/)

target_link_libraries(AIEXEC PUBLIC CommonLib)
if (WIN32)
    find_package(OpenGL REQUIRED)
    target_link_libraries(AIEXEC PUBLIC ${OPENGL_LIBRARIES})
    target_link_libraries(AIEXEC PUBLIC ${CMAKE_SOURCE_DIR}/lib/glew32.lib)
    target_link_libraries(AIEXEC PUBLIC ${CMAKE_SOURCE_DIR}/lib/glfw3.lib)
endif ()
