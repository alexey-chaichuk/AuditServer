#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QtDebug>
#include "netadapterwindow.h"
#include "ui_netadapterwindow.h"

NetAdapterWindow::NetAdapterWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetAdapterWindow)
{
    ui->setupUi(this);
}

NetAdapterWindow::~NetAdapterWindow()
{
    delete ui;
}


void NetAdapterWindow::updateTable()
{
    QSqlQuery query;
    QString strQuery =  "SELECT * "
                        "FROM netadapters "
                        "WHERE computer_id=? "
                        "ORDER BY mac";

    query.prepare(strQuery);
    query.bindValue(0, comp_id);
    qDebug() << "quering for comp_id = " << comp_id;
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
    ui->tableView->resizeColumnsToContents();
}
