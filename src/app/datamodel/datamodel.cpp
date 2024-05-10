#include "datamodel.h"

#include "dataprovider.h"
#include "taskrunner/taskrunner.h"

#include <QFile>
#include <QFileInfo>

namespace {
constexpr const char* ErrorMessage[]
    = { "Failed opening file", "Failed reading file", "Failed writing file",
        "Corrupted file" };
} // namespace

namespace model {
struct DataModel::Impl {
  Impl(DataModel& that, const char* path)
      : this_ { that },
        dataProvider_ { path } {
    dirData_ = dataProvider_.getFileInfo();
    connect(&taskRunner_, &concurrent::TaskRunner::taskFinished,
            [this](compres::Errors err, int index) {
              constexpr const char* ErrorMessage[]
                  = { "Failed opening file", "Failed reading file",
                      "Failed writing file", "Corrupted file" };
              if (err != compres::noError)
                emit this_.error(ErrorMessage[err], index);
              else
                emit this_.processingFinished();
            });
  }

  void compress(int index) {
    auto fileToCompress = dirData_[index].first.absoluteFilePath();
    auto resultFileName = dirData_[index].first.absolutePath() + "/"
                          + dirData_[index].first.baseName() + ".barch";
    taskRunner_.run(concurrent::TaskRunner::Task::compres, index,
                    fileToCompress, resultFileName);
  }

  void decompress(int index) {
    auto fileToCompress = dirData_[index].first.absoluteFilePath();
    auto resultFileName = dirData_[index].first.absolutePath() + "/"
                          + dirData_[index].first.baseName() + " unpacked.bmp";
    taskRunner_.run(concurrent::TaskRunner::Task::decompress, index,
                    fileToCompress, resultFileName);
  }

  DataModel&                     this_;
  DataProvider                   dataProvider_;
  concurrent::TaskRunner         taskRunner_;
  QList<QPair<QFileInfo, State>> dirData_;
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
