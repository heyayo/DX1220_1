project(DX1220 VERSION 1.0.0)

file(GLOB SOURCE_FILES *.cpp)

add_library(
        CommonLib
        ${SOURCE_FILES}
)
