#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QtDebug>
#include "softwaredetails.h"
#include "ui_softwaredetails.h"

SoftwareDetails::SoftwareDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoftwareDetails)
{
    ui->setupUi(this);
}

SoftwareDetails::~SoftwareDetails()
{
    delete ui;
}

void SoftwareDetails::updateTable()
{
    QSqlQuery query;
    QString strQuery =  "SELECT * "
                        "FROM software "
                        "WHERE computer_id=? "
                        "ORDER BY name ";

    query.prepare(strQuery);
    query.bindValue(0, comp_id);
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

void SoftwareDetails::on_txtSoftwareName_editingFinished()
{
    if(proxyModel) {
        QRegExp regExp(ui->txtSoftwareName->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
        proxyModel->setFilterRegExp(regExp);
        proxyModel->setFilterKeyColumn(2);
    }
}
