#ifndef COMPRES_COMPRESSOR_H
#define COMPRES_COMPRESSOR_H

#include "types.h"

#include <memory>

namespace compres {

class Compressor {
public:
  Compressor();
  ~Compressor();

  void processData(std::shared_ptr<Data> data);

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace compres

#endif // COMPRES_COMPRESSOR_H
