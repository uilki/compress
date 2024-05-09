#ifndef MODEL_DATAPROVIDER_H
#define MODEL_DATAPROVIDER_H

#include "abstractdatamodel.h"

#include <QDir>
#include <memory>

namespace model {

class DataProvider : public QObject {
  Q_OBJECT

public:
  explicit DataProvider(const QString& path);
  ~DataProvider();

  QList<QPair<QFileInfo, State>> getFileInfo();

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace model

#endif // MODEL_DATAPR)VIDER_H
