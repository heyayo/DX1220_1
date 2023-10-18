project(DX1220 VERSION 1.0.0)

file(GLOB SOURCE_FILES *.cpp)

set(CMAKE_EXE_LINKER_FLAGS "-lGL -lGLU -lGLEW -lglfw")

add_executable(
        AIEXEC
        ${SOURCE_FILES}
)

target_include_directories(AIEXEC PUBLIC ${CMAKE_SOURCE_DIR}/Common/)
target_include_directories(AIEXEC PUBLIC ${CMAKE_SOURCE_DIR}/include/)

target_link_libraries(AIEXEC PUBLIC CommonLib)
