#include "bmpimageprocessor.h"

#include "compressor.h"
#include "decompressor.h"

#include <fstream>
#include <iterator>
#include <vector>

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

// -----------------------------------------------------------------------------
template <typename Stream, typename T>
std::enable_if_t<std::is_arithmetic_v<T>> readFromStream(Stream& os, T& val) {
  T tmp;
  os.readsome(reinterpret_cast<typename Stream::char_type*>(&tmp), sizeof(T));
  val = tmp;
}

template <typename Stream, typename Cont>
std::enable_if_t<!std::is_arithmetic_v<Cont>> readFromStream(Stream& os,
                                                             Cont&   c) {
  typename Cont::size_type size;
  readFromStream(os, size);
  c.resize(size);
  for (auto& entry: c)
    readFromStream(os, entry);
}

template <typename Stream, typename T> void fromStream(Stream& os, T& val) {
  readFromStream(os, val);
}

template <typename Stream, typename First, typename... Rest>
void fromStream(Stream& os, First& val, Rest&... rest) {
  readFromStream(os, val);
  fromStream(os, rest...);
}

namespace compres {
struct BmpImageProcessor::Impl {
  enum Offsets {
    Start       = 0,
    FileSize    = 2,
    PixelArray  = 10,
    ImageWidth  = 18,
    ImageHeight = 22,
    ImageSize   = 34
  };

  Impl()
      : data_ { std::make_shared<Data>() } {};

  Errors readIntValue(std::ifstream& is, Offsets offset, int& value) {
    if (offset + sizeof(decltype(value)) > fileLength)
      return Errors::corruptedFile;

    is.seekg(offset);
    if (is.bad())
      return Errors::readError;

    is.readsome(intBuffer, std::size(intBuffer));
    if (is.bad())
      return Errors::readError;

    value = *(reinterpret_cast<int*>(intBuffer));
    return Errors::noError;
  }

  void internalClear() {
    imageSize    = 0;
    pixelsOffset = 0;
    fileSize     = 0;
    fileLength   = 0;
    data_->compressedData.clear();
    data_->emptyRows.clear();
    data_->headers.clear();
    data_->pixels.clear();
    data_->optional.clear();
  }

  bool readRawData(std::ifstream& is, int offset, std::vector<u_char>& vec) {
    is.seekg(offset);
    if (is.bad())
      return false;

    is.readsome(reinterpret_cast<char*>(vec.data()), vec.size());
    if (is.bad())
      return false;
    return true;
  }

  bool readPixels(std::ifstream& is) {
    auto currentPos = pixelsOffset;
    is.seekg(currentPos);
    if (is.bad())
      return false;

    int pixelSize   = imageSize / (data_->imageWidth * data_->imageHeight);
    int paddingSize = (imageSize / data_->imageHeight) % data_->imageWidth;
    for (int row = 0; row < data_->imageHeight; ++row) {
      for (int col = 0; col < data_->imageWidth; ++col) {
        char pixelBuffer[pixelSize];
        is.readsome(pixelBuffer, pixelSize);
        if (is.bad())
          return false;
        data_->pixels[row * data_->imageWidth + col] = pixelBuffer[0];
      }

      currentPos += data_->imageWidth * pixelSize + paddingSize;
      is.seekg(currentPos);
    }
    return true;
  }

  Errors parseHeaders(std::ifstream& is) {
    is.seekg(0, is.end);
    if (is.bad())
      return Errors::readError;

    fileLength = is.tellg();
    if (is.bad())
      return Errors::readError;

    if (auto err = readIntValue(is, Offsets::FileSize, fileSize);
        err != Errors::noError)
      return err;

    if (fileSize != fileLength)
      return Errors::corruptedFile;

    if (auto err = readIntValue(is, Offsets::PixelArray, pixelsOffset);
        err != Errors::noError) {
      return err;
    }

    if (pixelsOffset > fileSize)
      return Errors::corruptedFile;

    if (auto err = readIntValue(is, Offsets::ImageWidth, data_->imageWidth);
        err != Errors::noError) {
      return err;
    }
    if (auto err = readIntValue(is, Offsets::ImageHeight, data_->imageHeight);
        err != Errors::noError) {
      return err;
    }
    if (auto err = readIntValue(is, Offsets::ImageSize, imageSize);
        err != Errors::noError) {
      return err;
    }

    if (pixelsOffset + imageSize > fileLength)
      return Errors::corruptedFile;

    return Errors::noError;
  }

  Errors compressAndSaveImage(const char* inFileName,
                              const char* ouiFileName) {
    internalClear();

    std::ifstream is;
    is.open(inFileName);
    if (!is.is_open())
      return Errors::openFileError;

    if (auto err = parseHeaders(is); err != Errors::noError)
      return err;

    data_->headers.resize(pixelsOffset);
    if (!readRawData(is, Offsets::Start, data_->headers))
      return Errors::readError;

    data_->pixels.resize(data_->imageHeight * data_->imageWidth);
    if (!readPixels(is))
      return Errors::readError;

    data_->optional.resize(fileSize - pixelsOffset - imageSize);
    if (!readRawData(is, pixelsOffset + imageSize, data_->optional))
      return Errors::readError;
    Compressor comp;
    comp.processData(data_);
    return saveCompressed(ouiFileName);
  }

  Errors saveCompressed(const char* fileName) {
    std::ofstream os;
    os.open(fileName);
    if (!os.is_open())
      return Errors::openFileError;

    toStream(os, data_->headers, data_->compressedData, data_->emptyRows,
             data_->optional);
    if (os.bad())
      return Errors::writeError;
    return Errors::noError;
  }

  Errors decompressAndSaveImage(const char* inFileName,
                                const char* outFileName) {
    internalClear();

    std::ifstream is;
    is.open(inFileName);
    if (!is.is_open())
      return Errors::openFileError;

    fromStream(is, data_->headers, data_->compressedData, data_->emptyRows,
               data_->optional);
    if (is.bad())
      return Errors::readError;

    Decompressor dc;
    data_->imageWidth  = *(reinterpret_cast<const int*>(data_->headers.data()
                                                        + Offsets::ImageWidth));
    data_->imageHeight = *(reinterpret_cast<const int*>(
        data_->headers.data() + Offsets::ImageHeight));
    dc.processData(data_);
    return saveDecompressed(outFileName);
  }

  Errors saveDecompressed(const char* fileName) {
    std::ofstream os;
    os.open(fileName);
    if (!os.is_open())
      return Errors::openFileError;

    os.write(reinterpret_cast<const std::ofstream::char_type*>(
                 data_->headers.data()),
             data_->headers.size());
    if (os.bad())
      return Errors::writeError;
    int imageSize   = *(reinterpret_cast<const int*>(data_->headers.data()
                                                     + Offsets::ImageSize));
    int pixelSize   = imageSize / (data_->imageWidth * data_->imageHeight);
    int paddingSize = (imageSize / data_->imageHeight) % data_->imageWidth;
    for (int row = 0; row < data_->imageHeight; ++row) {
      for (int col = 0; col < data_->imageWidth; ++col) {
        for (int i = 0; i < pixelSize; ++i) {
          os.write(reinterpret_cast<const std::ofstream::char_type*>(
                       &data_->pixels[row * data_->imageWidth + col]),
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

    os.write(reinterpret_cast<const std::ofstream::char_type*>(
                 data_->optional.data()),
             data_->optional.size());
    if (os.bad())
      return Errors::writeError;
    return Errors::noError;
  }

private:
  char                  intBuffer[sizeof(int)] {};
  int                   fileLength {};
  int                   fileSize {};
  int                   pixelsOffset {};
  int                   imageSize {};
  std::shared_ptr<Data> data_;
};

BmpImageProcessor::BmpImageProcessor()
    : impl_ { std::make_unique<Impl>() } {}

BmpImageProcessor::~BmpImageProcessor() = default;

BmpImageProcessor::Errors
BmpImageProcessor::compressAndSaveImage(const char* inFileName,
                                        const char* outFileName) {
  return impl_->compressAndSaveImage(inFileName, outFileName);
}

BmpImageProcessor::Errors
BmpImageProcessor::decompressAndSaveImage(const char* inFileName,
                                          const char* outFileName) {
  return impl_->decompressAndSaveImage(inFileName, outFileName);
}

} // namespace compres
