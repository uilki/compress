#include "compressortask.h"

#include "bmpimageprocessor.h"

namespace concurrent {

CompressorTask::CompressorTask(QObject* parent)
    : QObject { parent } {}

void CompressorTask::compress(int index, const QString& in,
                              const QString& out) {
  auto err = compres::BmpImageProcessor::compressAndSaveImage(
      in.toStdString().c_str(), out.toStdString().c_str());
  emit finished(err, index);
}

void CompressorTask::decompess(int index, const QString& in,
                               const QString& out) {
  auto err = compres::BmpImageProcessor::decompressAndSaveImage(
      in.toStdString().c_str(), out.toStdString().c_str());
  emit finished(err, index);
}

} // namespace concurrent
