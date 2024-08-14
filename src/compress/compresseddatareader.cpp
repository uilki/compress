#include "compresseddatareader.h"

#include "logger.h"
#include <fstream>
// -----------------------------------------------------------------------------
template <typename Stream, typename T>
std::enable_if_t<std::is_arithmetic_v<T>> readFromStream(Stream &os, T &val) {
  T tmp;
  os.readsome(reinterpret_cast<typename Stream::char_type *>(&tmp), sizeof(T));
  val = tmp;
}

template <typename Stream, typename Cont>
std::enable_if_t<!std::is_arithmetic_v<Cont>> readFromStream(Stream &os,
                                                             Cont &c) {
  typename Cont::size_type size;
  readFromStream(os, size);
  c.resize(size);
  for (auto &entry : c)
    readFromStream(os, entry);
}

template <typename Stream, typename T> void fromStream(Stream &os, T &val) {
  readFromStream(os, val);
}

template <typename Stream, typename First, typename... Rest>
void fromStream(Stream &os, First &val, Rest &...rest) {
  readFromStream(os, val);
  fromStream(os, rest...);
}

namespace compres {
struct CompressedDataReader::Impl {
  Impl() = default;

  return_type prepareData(const char *fileName) {
    std::ifstream is;
    is.open(fileName);
    if (!is.is_open()) {
      log_info << "Failed to open " << fileName;
      return {Errors::openFileError, nullptr};
    }

    std::shared_ptr<compres::Data> data{new Data};

    fromStream(is, data->headers, data->compressedData, data->emptyRows,
               data->optional);
    if (is.bad()) {
      log_info << "Failed to read " << fileName;
      return {Errors::readError, nullptr};
    }

    data->imageWidth = *(reinterpret_cast<const int *>(data->headers.data() +
                                                       Offsets::ImageWidth));
    data->imageHeight = *(reinterpret_cast<const int *>(data->headers.data() +
                                                        Offsets::ImageHeight));
    return {Errors::noError, data};
  }
};

CompressedDataReader::CompressedDataReader()
    : impl_{std::make_unique<Impl>()} {}

CompressedDataReader::~CompressedDataReader() = default;

CompressedDataReader::return_type
CompressedDataReader::prepareData(const char *fileName) {
  return impl_->prepareData(fileName);
}

} // namespace compres
