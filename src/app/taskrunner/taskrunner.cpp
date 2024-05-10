#include "taskrunner.h"

#include <QtConcurrent>

namespace concurrent {

TaskRunner::TaskRunner(QObject* parent)
    : QObject { parent },
      pool_ { *QThreadPool::globalInstance() } {
  connect(&compressorTask_, &CompressorTask::finished, this,
          &TaskRunner::taskFinished, Qt::QueuedConnection);
}

void TaskRunner::run(Task task, int index, const QString& in,
                     const QString& out) {
  QtConcurrent::task(task == Task::compres ? &CompressorTask::compress
                                           : &CompressorTask::decompess)
      .onThreadPool(pool_)
      .withArguments(&compressorTask_, index, in, out)
      .spawn(QtConcurrent::FutureResult::Ignore);
}

} // namespace concurrent
