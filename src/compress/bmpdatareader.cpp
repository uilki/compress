#include "bmpdatareader.h"

#include <fstream>

namespace compres {
struct BmpDataReader::Impl {
  Impl() = default;

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

  Errors parseHeaders(std::ifstream&                  is,
                      std::shared_ptr<compres::Data>& data) {
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

    if (auto err = readIntValue(is, Offsets::ImageWidth, data->imageWidth);
        err != Errors::noError) {
      return err;
    }
    if (auto err = readIntValue(is, Offsets::ImageHeight, data->imageHeight);
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

  bool readRawData(std::ifstream& is, int offset, std::vector<u_char>& vec) {
    is.seekg(offset);
    if (is.bad())
      return false;

    is.readsome(reinterpret_cast<char*>(vec.data()), vec.size());
    if (is.bad())
      return false;
    return true;
  }

  bool readPixels(std::ifstream& is, std::shared_ptr<compres::Data>& data) {
    auto currentPos = pixelsOffset;
    is.seekg(currentPos);
    if (is.bad())
      return false;

    int pixelSize   = imageSize / (data->imageWidth * data->imageHeight);
    int paddingSize = (imageSize / data->imageHeight) % data->imageWidth;
    for (int row = 0; row < data->imageHeight; ++row) {
      for (int col = 0; col < data->imageWidth; ++col) {
        char pixelBuffer[pixelSize];
        is.readsome(pixelBuffer, pixelSize);
        if (is.bad())
          return false;
        data->pixels[row * data->imageWidth + col] = pixelBuffer[0];
      }

      currentPos += data->imageWidth * pixelSize + paddingSize;
      is.seekg(currentPos);
    }
    return true;
  }

  return_type prepareData(const char* fileName) {
    std::ifstream is;
    is.open(fileName);
    if (!is.is_open())
      return { Errors::openFileError, nullptr };

    std::shared_ptr<compres::Data> data { new Data };

    if (auto err = parseHeaders(is, data); err != Errors::noError)
      return { err, nullptr };

    data->headers.resize(pixelsOffset);
    if (!readRawData(is, Offsets::Start, data->headers))
      return { Errors::readError, nullptr };

    data->pixels.resize(data->imageHeight * data->imageWidth);
    if (!readPixels(is, data))
      return { Errors::readError, nullptr };

    data->optional.resize(fileSize - pixelsOffset - imageSize);
    if (!readRawData(is, pixelsOffset + imageSize, data->optional))
      return { Errors::readError, nullptr };

    return { Errors::noError, data };
  }

  char intBuffer[sizeof(int)] {};
  int  fileLength {};
  int  fileSize {};
  int  pixelsOffset {};
  int  imageSize {};
};

BmpDataReader::BmpDataReader()
    : impl_ { std::make_unique<Impl>() } {}

BmpDataReader::~BmpDataReader() = default;

return_type BmpDataReader::prepareData(const char* fileName) {
  return impl_->prepareData(fileName);
}

} // namespace compres
