#ifndef COMPRESSORTASK_H
#define COMPRESSORTASK_H

#include "types.h"

#include <QObject>

namespace concurrent {

class CompressorTask : public QObject {
  Q_OBJECT

public:
  CompressorTask(QObject* parent = nullptr);

  void compress(int index, const QString& in, const QString& out);
  void decompess(int index, const QString& in, const QString& out);

signals:
  void finished(compres::Errors err, int index);
};

} // namespace concurrent

#endif // COMPRESSORTASK_H
