#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QtDebug>
#include <QProcess>
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    softwareDetailsWindow = 0;
    netAdapterWindow = 0;
    softwareFindWindow = 0;
    QDateTime date_end = QDateTime::currentDateTime();
    ui->date_end->setDateTime(date_end.addDays(+1));
    ui->date_beg->setDateTime(date_end.addDays(-8));
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
                        "RIGHT JOIN ( "
                            "SELECT ui.* "
                            "FROM ( "
                                "SELECT MAX(id) AS mid "
                                "FROM users "
                                "WHERE DATE(loggedin) BETWEEN ? AND ?"
                                "GROUP BY computername "
                            ") uo "
                            "JOIN users ui "
                            "ON ui.id = uo.mid "
                        ") uu "
                        "ON mi.name = uu.computername "
                        "ORDER BY uu.loggedin";

    query.prepare(strQuery);
    query.bindValue(0, ui->date_beg->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(1, ui->date_end->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
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
        } else if(index.column() == 8) {
            //QString program = "xfreerdp";
            QString program = "rdesktop";
            QString login = index.data().toString();
            QString username, domain, name, domain_name;
            QStringList arguments;
            if (login.length() > 0) {
                arguments << "-g 1280x1024";
                arguments << "-D";
                //arguments << "--no-auth";
                //arguments << "--ignore-certificate";
                //arguments << "--no-nla";
                name = proxyModel->data(proxyModel->index(index.row(),2)).toString();
                domain_name = proxyModel->data(proxyModel->index(index.row(),3)).toString();
                int domain_separator = login.indexOf("@");
                if(domain_separator > 0) username = login.left(domain_separator);
                arguments << ("-u" + username.trimmed());
                int dot_separator = login.indexOf(".", domain_separator);
                if(dot_separator > 0) domain = login.mid(domain_separator + 1, dot_separator - domain_separator - 1);
                arguments << ("-d" + domain.trimmed());
                arguments << (name.trimmed() + "." + domain_name.trimmed());
                qDebug() << arguments;
                QProcess *rdp_proc = new QProcess();
                rdp_proc->start(program, arguments);
                rdp_proc->waitForFinished(500);
                QByteArray rdp_proc_out = rdp_proc->readAllStandardOutput();
                QByteArray rdp_proc_err = rdp_proc->readAllStandardError();
                ui->outText->appendPlainText("rdesktop " + arguments.join(" "));
                if(rdp_proc_out.trimmed() != "") ui->outText->appendPlainText(rdp_proc_out.trimmed());
                if(rdp_proc_err.trimmed() != "") ui->outText->appendPlainText(rdp_proc_err.trimmed());
            }
        }
    }
}

void MainWindow::on_btnFindSoftware_clicked()
{
    if(softwareFindWindow !=0) {
        softwareFindWindow->close();
        delete softwareFindWindow;
        softwareFindWindow = 0;
    }
    softwareFindWindow = new SoftwareFind(this);
    softwareFindWindow->soft_name = ui->txtSoftName->text();
    softwareFindWindow->date_beg = ui->date_beg->dateTime();
    softwareFindWindow->date_end = ui->date_end->dateTime();
    softwareFindWindow->updateTable();
    softwareFindWindow->show();
}
