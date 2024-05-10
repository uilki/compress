#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "abstractdatamodel.h"

#include <memory>

namespace model {

class DataModel : public AbstractDataModel {
  Q_OBJECT

public:
  explicit DataModel(const char* path, QObject* parent = nullptr);
  ~DataModel();

  virtual QList<QPair<QFileInfo, State>>& getData() override;

public slots:
  virtual void compress(int index) override;
  virtual void decompress(int index) override;

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace model
#endif // DATAMODEL_H
