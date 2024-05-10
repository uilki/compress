#ifndef BMPDATAREADER_H
#define BMPDATAREADER_H

#include "types.h"

#include <memory>

namespace compres {
using return_type = std::pair<Errors, std::shared_ptr<compres::Data>>;

class BmpDataReader {
public:
  BmpDataReader();
  ~BmpDataReader();
  return_type prepareData(const char* fileName);

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace compres

#endif // BMPDATAREADER_H
