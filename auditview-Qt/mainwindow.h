#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "softwaredetails.h"
#include "softwarefind.h"
#include "netadapterwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_btnOpen_clicked();

    void on_txtLoginFilter_editingFinished();

    void on_txtPCNameFilter_editingFinished();

    void on_txtIPAddrFilter_editingFinished();

    void on_btnRefresh_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_btnFindSoftware_clicked();

private:
    Ui::MainWindow *ui;
    QSortFilterProxyModel *proxyModel;
    SoftwareDetails *softwareDetailsWindow;
    NetAdapterWindow *netAdapterWindow;
    SoftwareFind *softwareFindWindow;
};

#endif // MAINWINDOW_H
