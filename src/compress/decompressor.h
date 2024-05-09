#ifndef COMPRES_DECOMPRESSOR_H
#define COMPRES_DECOMPRESSOR_H

#include <memory>

namespace compres {

struct Data;

class Decompressor {
public:
  Decompressor();
  ~Decompressor();

  void processData(std::shared_ptr<Data> data);

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace compres

#endif // COMPRES_DECOMPRESSOR_H
