#include "saveutils.h"

#include <fstream>

template <typename T>
std::enable_if_t<std::is_arithmetic_v<T>> writeToStream(std::ostream& os,
                                                        const T&      v) {
  os.write(reinterpret_cast<const std::ostream::char_type*>(&v), sizeof(T));
}

template <typename Cont>
std::enable_if_t<sizeof(typename Cont::value_type)
                 != sizeof(typename std::ostream::char_type)>
writeToStream(std::ostream& os, const Cont& c) {
  auto size = c.size();
  writeToStream(os, size);
  for (const auto& e: c)
    writeToStream(os, e);
}

template <typename Cont>
std::enable_if_t<sizeof(typename Cont::value_type)
                 == sizeof(typename std::ostream::char_type)>
writeToStream(std::ostream& os, const Cont& c) {
  auto size = c.size();
  writeToStream(os, size);
  os.write(reinterpret_cast<const std::ostream::char_type*>(c.data()),
           c.size());
}

template <typename T> void toStream(std::ostream& os, const T& v) {
  writeToStream(os, v);
}

template <typename First, typename... Rest>
void toStream(std::ostream& os, const First& val, const Rest&... rest) {
  writeToStream(os, val);
  toStream(os, rest...);
}

namespace compres {
Errors saveCompressed(const char*                  fileName,
                      const std::shared_ptr<Data>& data) {
  std::ofstream os;
  os.open(fileName);
  if (!os.is_open())
    return Errors::openFileError;

  toStream(os, data->headers, data->compressedData, data->emptyRows,
           data->optional);
  if (os.bad())
    return Errors::writeError;
  return Errors::noError;
}

Errors saveDecompressed(const char*                  fileName,
                        const std::shared_ptr<Data>& data) {
  std::ofstream os;
  os.open(fileName);
  if (!os.is_open())
    return Errors::openFileError;

  os.write(
      reinterpret_cast<const std::ofstream::char_type*>(data->headers.data()),
      data->headers.size());
  if (os.bad())
    return Errors::writeError;
  int imageSize = *(
      reinterpret_cast<const int*>(data->headers.data() + Offsets::ImageSize));
  int pixelSize   = imageSize / (data->imageWidth * data->imageHeight);
  int paddingSize = (imageSize / data->imageHeight) % data->imageWidth;
  for (int row = 0; row < data->imageHeight; ++row) {
    for (int col = 0; col < data->imageWidth; ++col) {
      for (int i = 0; i < pixelSize; ++i) {
        os.write(reinterpret_cast<const std::ofstream::char_type*>(
                     &data->pixels[row * data->imageWidth + col]),
                 1);
        if (os.bad())
          return Errors::writeError;
      }
    }

    char paddingValue = 0;
    os.write(&paddingValue, paddingSize);
    if (os.bad())
      return Errors::writeError;
  }

  os.write(
      reinterpret_cast<const std::ofstream::char_type*>(data->optional.data()),
      data->optional.size());
  if (os.bad())
    return Errors::writeError;
  return Errors::noError;
}

} // namespace compres
