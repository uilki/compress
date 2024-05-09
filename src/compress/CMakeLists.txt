set(TARGET "compress")

file(GLOB SOURCES
    *.h
    *.cpp
)

add_library(${TARGET} STATIC ${SOURCES})

