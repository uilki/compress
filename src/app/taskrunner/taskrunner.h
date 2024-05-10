#ifndef TASKRUNNER_H
#define TASKRUNNER_H

#include "compressortask.h"
#include "types.h"

#include <QObject>
#include <QThreadPool>

namespace concurrent {

class TaskRunner : public QObject {
  Q_OBJECT

public:
  enum class Task { compres, decompress };
  explicit TaskRunner(QObject* parent = nullptr);

  void run(Task task, int index, const QString& in, const QString& out);

signals:
  void taskFinished(compres::Errors err, int index);

private:
  CompressorTask compressorTask_;
  QThreadPool&   pool_;
};

} // namespace concurrent

#endif // TASKRUNNER_H
