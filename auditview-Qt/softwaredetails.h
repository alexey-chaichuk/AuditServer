#ifndef SOFTWAREDETAILS_H
#define SOFTWAREDETAILS_H

#include <QDialog>
#include <QSortFilterProxyModel>

namespace Ui {
class SoftwareDetails;
}

class SoftwareDetails : public QDialog
{
    Q_OBJECT
    
public:
    explicit SoftwareDetails(QWidget *parent = 0);
    ~SoftwareDetails();
    int comp_id;
    void updateTable();
    
private slots:
    void on_txtSoftwareName_editingFinished();

private:
    Ui::SoftwareDetails *ui;
    QSortFilterProxyModel *proxyModel;
};

#endif // SOFTWAREDETAILS_H
