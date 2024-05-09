#ifndef COMPRES_DATAPROWIDER_H
#define COMPRES_DATAPROWIDER_H

#include <memory>

namespace compres {

class BmpImageProcessor {
public:
  enum Errors { noError, openFileError, readError, writeError, corruptedFile };

public:
  BmpImageProcessor();
  ~BmpImageProcessor();
  Errors compressAndSaveImage(const char* inFileName, const char* outFileName);
  Errors decompressAndSaveImage(const char* inFileName,
                                const char* outFileName);

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace compres

#endif // COMPRES_DATAPROWIDER_H
