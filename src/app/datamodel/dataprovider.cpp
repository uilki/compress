#include "dataprovider.h"

#include "abstractdatamodel.h"

#include <QDir>

namespace {
QSet<QString> suffixes { "bmp", "barch", "png" };
}

namespace model {
struct DataProvider::Impl {
  Impl(const QString& path)
      : dir { path } {
    if (!dir.exists())
      dir.setPath("");
  }

  QList<QPair<QFileInfo, model::State>> getFileInfo() {
    QList<QPair<QFileInfo, State>> res;
    dir.refresh();
    for (const auto& entry: dir.entryInfoList(
             QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks)) {
      if (suffixes.contains(entry.completeSuffix()))
        res << QPair<QFileInfo, State> { entry, State::idle };
    }
    return res;
  }

  QDir dir;
};
} // namespace model

model::DataProvider::DataProvider(const QString& path)
    : impl_ { std::make_unique<Impl>(path) } {}

model::DataProvider::~DataProvider() = default;

QList<QPair<QFileInfo, model::State>> model::DataProvider::getFileInfo() {
  return impl_->getFileInfo();
}
