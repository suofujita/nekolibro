#ifndef IMEXPORT_H
#define IMEXPORT_H

#include <QWidget>

#include "nekolibro.h"
#include "categorieswindow.h"
namespace Ui {
class ImExport;
}

class ImExport : public QWidget
{
    Q_OBJECT

public:
    explicit ImExport(QWidget *parent = nullptr);
    ~ImExport();
public slots:
    void toMainImExport();
    void gotoImportInvoice();
    void gotoExportInvoice();
    void gotoImportLogs();
    void gotoExportLogs();
private slots:
    void saveImportInvoices();
    void viewImportLogs();
    void toAddNewCategory();
    void delayTabImport();
    void searchBooks(const QString &text);
    void removeBooksFromImportTable(int row);
    //void viewEmportLogs();
    //void saveExportInvoice();


private:
    Ui::ImExport *ui;
    CategoriesWindow *pCategoriesWindow = nullptr;
    QSqlDatabase db;
    int totalQuantity = 0;
    void showTime();                         // check 6.32
    void autoCreateBillNum();                // check 6.32
    void selectedBooks(QAction *action);
    void setCompleter();
    void updateTotals();
    int getInvoiceId(const QString &numBill);
    void updatedStock();
};

#endif // IMEXPORT_H
