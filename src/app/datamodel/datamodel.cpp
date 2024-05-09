#include "datamodel.h"

#include "bmpimageprocessor.h"
#include "dataprovider.h"

#include <QFile>
#include <QFileInfo>
#include <mutex>

namespace {
constexpr const char* ErrorMessage[]
    = { "", "Failed opening file", "Failed reading file",
        "Failed writing file", "Corrupted file" };
} // namespace

namespace model {
struct DataModel::Impl {
  Impl(DataModel& that, const char* path)
      : this_ { that },
        dataProvider_ { path } {
    dirData_ = dataProvider_.getFileInfo();
  }

  void compress(int index) {
    std::unique_lock<std::mutex> lk { processMutex_, std::defer_lock };
    if (!lk.try_lock()) {
      emit this_.error("Another file is being processed...", index);
      return;
    }
    auto compressedFileName = dirData_[index].first.absolutePath() + "/"
                              + dirData_[index].first.baseName() + ".barch";
    if (auto err = imageProcessor_.compressAndSaveImage(
            dirData_[index].first.absoluteFilePath().toStdString().c_str(),
            compressedFileName.toStdString().c_str());
        err != compres::BmpImageProcessor::noError) {
      emit this_.error(ErrorMessage[err], index);
      return;
    }

    emit this_.processingFinished();
  }

  void decompress(int index) {
    std::unique_lock<std::mutex> lk { processMutex_, std::defer_lock };
    if (!lk.try_lock()) {
      emit this_.error("Another file is being processed...", index);
      return;
    }

    auto decompressedFileName = dirData_[index].first.absolutePath() + "/"
                                + dirData_[index].first.baseName()
                                + " unpacked.bmp";
    if (auto err = imageProcessor_.decompressAndSaveImage(
            dirData_[index].first.absoluteFilePath().toStdString().c_str(),
            decompressedFileName.toStdString().c_str());
        err != compres::BmpImageProcessor::noError) {
      emit this_.error(ErrorMessage[err], index);
      return;
    }

    emit this_.processingFinished();
  }

  DataModel&                     this_;
  compres::BmpImageProcessor     imageProcessor_;
  DataProvider                   dataProvider_;

  QList<QPair<QFileInfo, State>> dirData_;
  std::mutex                     processMutex_;
}; // struct DataModel::Impl

DataModel::DataModel(const char* path, QObject* parent)
    : AbstractDataModel { parent },
      impl_ { std::make_unique<Impl>(*this, path) } {
  connect(this, &DataModel::processingFinished,
          [this]() { impl_->dirData_ = impl_->dataProvider_.getFileInfo(); });
}

DataModel::~DataModel() = default;

QList<QPair<QFileInfo, State>>& model::DataModel::getData() {
  return impl_->dirData_;
}

void model::DataModel::compress(int index) { impl_->compress(index); }

void model::DataModel::decompress(int index) { impl_->decompress(index); }

} // namespace model
