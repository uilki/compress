#ifndef COMPRESSEDDATAREADER_H
#define COMPRESSEDDATAREADER_H

#include "types.h"

#include <memory>

namespace compres {

class CompressedDataReader {
  using return_type = std::pair<Errors, std::shared_ptr<compres::Data>>;

public:
  CompressedDataReader();
  ~CompressedDataReader();

  return_type prepareData(const char* fileName);

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace compres

#endif // COMPRESSEDDATAREADER_H
