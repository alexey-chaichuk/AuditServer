#ifndef NETADAPTERWINDOW_H
#define NETADAPTERWINDOW_H

#include <QDialog>
#include <QSortFilterProxyModel>

namespace Ui {
class NetAdapterWindow;
}

class NetAdapterWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit NetAdapterWindow(QWidget *parent = 0);
    ~NetAdapterWindow();
    int comp_id;
    void updateTable();

private:
    Ui::NetAdapterWindow *ui;
    QSortFilterProxyModel *proxyModel;
};

#endif // NETADAPTERWINDOW_H
