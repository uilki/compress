set(TARGET "compress")

set(SOURCES
    bmpimageprocessor.h
    bmpdatareader.cpp
    decompressor.cpp
    bmpdatareader.h
    saveutils.h
    bmpimageprocessor.cpp
    types.h
    saveutils.cpp
    compressor.h
    compressor.cpp
    decompressor.h
    compresseddatareader.h
    compresseddatareader.cpp)

include_directories(../logging)

add_library(${TARGET} STATIC ${SOURCES})
