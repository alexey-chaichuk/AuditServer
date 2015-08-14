#ifndef SOFTWAREFIND_H
#define SOFTWAREFIND_H

#include <QDialog>
#include <QDateTime>
#include <QSortFilterProxyModel>

namespace Ui {
class SoftwareFind;
}

class SoftwareFind : public QDialog
{
    Q_OBJECT

public:
    explicit SoftwareFind(QWidget *parent = 0);
    ~SoftwareFind();
    QString soft_name;
    QDateTime date_beg;
    QDateTime date_end;
    void updateTable();

private:
    Ui::SoftwareFind *ui;
    QSortFilterProxyModel *proxyModel;
};

#endif // SOFTWAREFIND_H
