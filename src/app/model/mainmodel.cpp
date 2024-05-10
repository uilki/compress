#include "mainmodel.h"

#include "datamodel/datamodel.h"

namespace model {
MainModel::MainModel(const char* path)
    : dataModel_ { std::make_unique<DataModel>(path) } {
  connect(
      dataModel_.get(), &DataModel::processingFinished, this,
      [this]() {
        beginResetModel();
        endResetModel();
      },
      Qt::QueuedConnection);
  connect(
      dataModel_.get(), &DataModel::error, this,
      [this](const QString mes, int index) {
        changeState(index, idle);
        emit errorMessage(mes, index);
      },
      Qt::QueuedConnection);
}

QString MainModel::errorMes() const { return errorMes_; }

int     MainModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent);
  return dataModel_->getData().count();
}

QVariant MainModel::data(const QModelIndex& index, int role) const {
  if (index.row() < 0 || index.row() >= dataModel_->getData().count())
    return QVariant();

  switch (role) {
  case DataRoles::NameRole:
    return dataModel_->getData()[index.row()].first.baseName();
  case DataRoles::SizeRole:
    return dataModel_->getData()[index.row()].first.size();
  case DataRoles::PathRole:
    return dataModel_->getData()[index.row()].first.absolutePath();
  case DataRoles::SuffixRole:
    return dataModel_->getData()[index.row()].first.suffix();
  case DataRoles::ProcessingRole:
    switch (dataModel_->getData()[index.row()].second) {
    case State::compressing:
      return "compressing";
    case State::decompressing:
      return "decompressing";
    default:
      return "";
    }
  default:
    return {};
  }
}

QString MainModel::getFilename(int index) const {
  return dataModel_->getData()[index].first.absoluteFilePath();
}

void MainModel::changeState(int index, State state) {
  dataModel_->getData()[index].second = state;
  emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

QHash<int, QByteArray> MainModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[DataRoles::NameRole]       = "Name";
  roles[DataRoles::SizeRole]       = "Size";
  roles[DataRoles::PathRole]       = "FilePath";
  roles[DataRoles::SuffixRole]     = "Suffix";
  roles[DataRoles::ProcessingRole] = "Processing";
  return roles;
}

void MainModel::processFile(int index) {
  if (index < 0 || index >= dataModel_->getData().count())
    return;

  const auto& ext = dataModel_->getData()[index].first.suffix();
  if (ext != "barch") {
    changeState(index, State::compressing);
    dataModel_->compress(index);
  }
  else {
    changeState(index, State::decompressing);
    dataModel_->decompress(index);
  }
}

} // namespace model
