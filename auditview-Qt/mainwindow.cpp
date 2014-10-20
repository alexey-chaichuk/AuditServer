#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QtDebug>
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    softwareDetailsWindow = 0;
    netAdapterWindow = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnOpen_clicked()
{
    if(!QSqlDatabase::database().isValid()) {
        qDebug() << "default database is invalid, so opening it first time";
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("192.168.0.155");
        db.setDatabaseName("audit");
        db.setUserName("audit");
        db.setPassword("audit");
    }

    if(!QSqlDatabase::database().isOpen()) {
        qDebug() << "Opening database";
        if(!QSqlDatabase::database().open()) {
            qDebug() << QSqlDatabase::database().lastError();
            return;
        }
    }

    QSqlQuery query;
    QString strQuery =  "SELECT mi.*, uu.login, uu.loggedin "
                        "FROM ( "
                            "SELECT MAX(id) AS mid "
                            "FROM computers "
                            "GROUP BY name "
                        ") mo "
                        "JOIN computers mi "
                        "ON mi.id = mo.mid "
                        "LEFT JOIN ( "
                            "SELECT ui.* "
                            "FROM ( "
                                "SELECT MAX(id) AS mid "
                                "FROM users "
                                "GROUP BY computername "
                            ") uo "
                            "JOIN users ui "
                            "ON ui.id = uo.mid "
                        ") uu "
                        "ON mi.name = uu.computername "
                        "ORDER BY uu.loggedin";

    query.prepare(strQuery);
    if(!query.exec()) {
        qDebug() << "query.exec failed";
        qDebug() << query.lastError();
        qDebug() << "ReOpening database";
        if(!QSqlDatabase::database().open()) {
            qDebug() << QSqlDatabase::database().lastError();
            return;
        }
        qDebug() << "ReExecing query";
        if(!query.exec()) {
            qDebug() << "query.exec failed again";
            qDebug() << query.lastError();
        }
    }

    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(query);
    proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(model);
    ui->tableView->setModel(proxyModel);
    //((QSqlQueryTableModel*)ui->tableView->model())
    ui->tableView->resizeColumnsToContents();
}


void MainWindow::on_txtLoginFilter_editingFinished()
{
    if(proxyModel) {
        QRegExp regExp(ui->txtLoginFilter->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
        proxyModel->setFilterRegExp(regExp);
        proxyModel->setFilterKeyColumn(8);
    }
}

void MainWindow::on_txtPCNameFilter_editingFinished()
{
    if(proxyModel) {
        QRegExp regExp(ui->txtPCNameFilter->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
        proxyModel->setFilterRegExp(regExp);
        proxyModel->setFilterKeyColumn(2);
    }
}

void MainWindow::on_txtIPAddrFilter_editingFinished()
{
    if(proxyModel) {
        QRegExp regExp(ui->txtIPAddrFilter->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
        proxyModel->setFilterRegExp(regExp);
        proxyModel->setFilterKeyColumn(1);
    }
}

void MainWindow::on_btnRefresh_clicked()
{
    on_btnOpen_clicked();
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(index.isValid()) {
        if(index.column() == 0) {
            if(softwareDetailsWindow != 0) {
                softwareDetailsWindow->close();
                delete softwareDetailsWindow;
                softwareDetailsWindow = 0;
            }
            softwareDetailsWindow = new SoftwareDetails(this);
            softwareDetailsWindow->comp_id = index.data().toInt();
            softwareDetailsWindow->updateTable();
            qDebug() << index.data().toInt();;
            softwareDetailsWindow->show();
        } else if(index.column() == 1) {
            if(netAdapterWindow != 0) {
                netAdapterWindow->close();
                delete netAdapterWindow;
                netAdapterWindow = 0;
            }
            netAdapterWindow = new NetAdapterWindow(this);
            netAdapterWindow->comp_id = proxyModel->data(proxyModel->index(index.row(),0)).toInt();
            netAdapterWindow->updateTable();
            netAdapterWindow->show();
        }
    }
}
