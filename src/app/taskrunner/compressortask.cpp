#include "compressortask.h"

#include "bmpimageprocessor.h"
#include "logger.h"

#include <cassert>

std::string stringError(compres::Errors err) {
  switch (err) {
  case compres::Errors::noError:
    return "noError";
  case compres::Errors::openFileError:
    return "openFileError";
  case compres::Errors::corruptedFile:
    return "corruptedFile";
  case compres::Errors::readError:
    return "readError";
  case compres::Errors::writeError:
    return "writeError";
  default:
    assert("Unknown error code");
  }
}

namespace concurrent {

CompressorTask::CompressorTask(QObject *parent) : QObject{parent} {}

void CompressorTask::compress(int index, const QString &in,
                              const QString &out) {
  log_info << "Compress task started";
  auto err = compres::BmpImageProcessor::compressAndSaveImage(
      in.toStdString().c_str(), out.toStdString().c_str());
  log_info << "Compress task finished with error code: " << stringError(err);
  emit finished(err, index);
}

void CompressorTask::decompess(int index, const QString &in,
                               const QString &out) {
  log_info << "Decompress task started";
  auto err = compres::BmpImageProcessor::decompressAndSaveImage(
      in.toStdString().c_str(), out.toStdString().c_str());
  log_info << "Decompress task finished with error code: " << stringError(err);
  emit finished(err, index);
}

} // namespace concurrent
