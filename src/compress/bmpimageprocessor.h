#ifndef COMPRES_BMPIMAGEPROCESSOR_H
#define COMPRES_BMPIMAGEPROCESSOR_H

#include "types.h"

namespace compres {

class BmpImageProcessor {
public:
  static Errors compressAndSaveImage(const char* inFileName,
                                     const char* outFileName);
  static Errors decompressAndSaveImage(const char* inFileName,
                                       const char* outFileName);
};

} // namespace compres

#endif // COMPRES_BMPIMAGEPROCESSOR_H
