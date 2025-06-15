#ifndef VIEWINVOICESDETAILS_H
#define VIEWINVOICESDETAILS_H

#include <QDialog>
#include "nekolibro.h"

class Reports;
namespace Ui {
class ViewInvoicesDetails;
}

enum InvoiceType {
   Retail, Import, Export
};

class ViewInvoicesDetails : public QDialog
{
    Q_OBJECT

public:
    explicit ViewInvoicesDetails(const QString &invoiceId, InvoiceType type,QWidget *parent = nullptr);
    ~ViewInvoicesDetails();
public slots:
    void editInvoice();
    void saveEditInvoice();
    void closeTab();
    void exportLogsExcel();
private:
    Ui::ViewInvoicesDetails *ui;
    QSqlDatabase db;
    QString invoiceIdCurrent;
    InvoiceType InvoiceTypeCurrent;
    void loadInvoiceDetails();

};

#endif // VIEWINVOICESDETAILS_H
