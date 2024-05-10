#ifndef COMPRES_TYPES_H
#define COMPRES_TYPES_H

#include <vector>

namespace compres {
enum Errors {
  noError = -1,
  openFileError,
  readError,
  writeError,
  corruptedFile
};

enum Offsets {
  Start       = 0,
  FileSize    = 2,
  PixelArray  = 10,
  ImageWidth  = 18,
  ImageHeight = 22,
  ImageSize   = 34
};

enum class Colors : unsigned short { black = 0x00, white = 0xff };

enum Mark : unsigned short { white = 0x00, black = 0x02, other = 0x03 };

struct RawImageData {
  int            width;
  int            height;
  unsigned char* data;
};

struct Data {
  std::vector<unsigned char> headers;
  std::vector<unsigned char> pixels;
  std::vector<unsigned char> optional;
  std::vector<unsigned char> compressedData;
  std::vector<int>           emptyRows;
  int                        imageWidth {};
  int                        imageHeight {};
};
} // namespace compres

#endif // COMPRES_TYPES_H
