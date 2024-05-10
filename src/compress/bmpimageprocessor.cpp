#include "bmpimageprocessor.h"

#include "bmpdatareader.h"
#include "compresseddatareader.h"
#include "compressor.h"
#include "decompressor.h"
#include "saveutils.h"

namespace compres {

Errors BmpImageProcessor::compressAndSaveImage(const char* inFileName,
                                               const char* ouiFileName) {
  BmpDataReader reader;
  auto [err, data] = reader.prepareData(inFileName);

  if (err != Errors::noError)
    return err;

  Compressor comp;
  comp.processData(data);
  return saveCompressed(ouiFileName, data);
}

Errors BmpImageProcessor::decompressAndSaveImage(const char* inFileName,
                                                 const char* outFileName) {
  CompressedDataReader reader;
  auto [err, data] = reader.prepareData(inFileName);

  if (err != Errors::noError)
    return err;

  Decompressor dc;
  dc.processData(data);
  return saveDecompressed(outFileName, data);
}

} // namespace compres
