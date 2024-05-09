#ifndef MAINMODEL_H
#define MAINMODEL_H

#include "abstractdatamodel.h"

#include <QAbstractListModel>

namespace model {

class MainModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum DataRoles {
    NameRole = Qt::UserRole + 1,
    SizeRole,
    StateRole,
    PathRole,
    SuffixRole,
    ProcessingRole
  };

public:
  explicit MainModel(const char* path);
  ~MainModel() = default;

  QString errorMes() const;

public:
  virtual int      rowCount(const QModelIndex& parent) const override;
  virtual QVariant data(const QModelIndex& index, int role) const override;
  virtual QHash<int, QByteArray> roleNames() const override;

signals:
  void processing(QString mes);
  void errorMessage(QString mes, int index);

public slots:
  void    processFile(int index);
  QString getFilename(int index) const;

private:
  void changeState(int index, State state);

private:
  std::shared_ptr<AbstractDataModel> dataModel_;
  QString                            errorMes_;
};

} // namespace model

#endif // MAINMODEL_H
