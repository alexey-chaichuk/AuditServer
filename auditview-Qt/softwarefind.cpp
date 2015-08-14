#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QtDebug>
#include "softwarefind.h"
#include "ui_softwarefind.h"

SoftwareFind::SoftwareFind(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoftwareFind)
{
    ui->setupUi(this);
}

SoftwareFind::~SoftwareFind()
{
    delete ui;
}

void SoftwareFind::updateTable()
{
    QSqlQuery query;
    QString strQuery =  "SELECT mi.name, mi.domain, mi.updated, uu.login, soft.name "
                        "FROM ( "
                            "SELECT MAX(id) AS mid "
                            "FROM computers "
                            "GROUP BY name "
                        ") mo "
                        "JOIN ( "
                            "SELECT * "
                            "FROM computers "
                            "WHERE DATE(updated) BETWEEN ? AND ? "
                        ") mi "
                        "ON mi.id = mo.mid "
                        "JOIN ("
                            "SELECT computer_id, name "
                            "FROM software "
                            "WHERE name LIKE ? "
                            "ORDER BY name "
                        ") soft "
                        "ON soft.computer_id=mi.id "
                        "LEFT JOIN ( "
                            "SELECT ui.* "
                            "FROM ( "
                                "SELECT MAX(id) AS mid "
                                "FROM users "
                                "WHERE DATE(loggedin) BETWEEN ? AND ? "
                                "GROUP BY computername "
                            ") uo "
                            "JOIN users ui "
                            "ON ui.id = uo.mid "
                        ") uu "
                        "ON mi.name = uu.computername "
                        "ORDER BY soft.name";

    query.prepare(strQuery);
    query.bindValue(0, date_beg.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(1, date_end.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(2, "%" + soft_name.toUtf8() + "%");
    query.bindValue(3, date_beg.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(4, date_end.toString("yyyy-MM-dd hh:mm:ss"));
    qDebug() << strQuery;
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
