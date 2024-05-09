#ifndef ABSTRACTDATAMODEL_H
#define ABSTRACTDATAMODEL_H

#include "qfileinfo.h"

#include <QObject>

namespace model {
enum State { idle, compressing, decompressing };

class AbstractDataModel : public QObject {
  Q_OBJECT

public:
  explicit AbstractDataModel(QObject* parent = nullptr)
      : QObject { parent } {};
  virtual ~AbstractDataModel()                      = default;
  virtual QList<QPair<QFileInfo, State>>& getData() = 0;

signals:
  void processingFinished();
  void error(QString mes, int index);
public slots:
  virtual void compress(int index)   = 0;
  virtual void decompress(int index) = 0;
};
} // namespace model
#endif // ABSTRACTDATAMODEL_H
