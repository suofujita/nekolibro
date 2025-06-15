#ifndef IMEXPORT_H
#define IMEXPORT_H

#include <QWidget>
#include "viewinvoicesdetails.h"
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
    void delayTab();
    void closeTab();
    void searchBooksForImport(const QString &text);
    void removeBooksFromImportTable();

    void viewExportLogs();
    void saveExportInvoice();
    void searchBooksForExport(const QString &text);
    void removeBooksFromExportTable();

    /* Xem lại lịch sử nhập hàng*/
    void resultsImportLogs();
    void loadAllImportInvoices();
    /* Xem lại lịch sử xuất hàng */
    void resultsExportLogs();
    void loadAllExportInvoices();
    /* xem chi tiết đơn hàng */
    void clickedImportBillNum(int row, int col);
    void clickedExportBillNum(int row, int col);

    /* thêm sản phẩm để xuất-nhập tách ra để xử lý trùng lặp khi thêm sản phẩm*/
    void addProductForImport(const QString &productId, const QString &name, const QString &author);
    void addProductForExport(const QString &productId, const QString &name, const QString &author, int stock);
private:
    Ui::ImExport *ui;
    CategoriesWindow *pCategoriesWindow = nullptr;
    ViewInvoicesDetails *pViewInvoicesDetails = nullptr;
    QSqlDatabase db;
    QTimer *time;
    int totalQuantityImport = 0, totalQuantityExport = 0;
    void showTime();                         // check 6.32
    void autoCreateImportBillNum();                // check 6.32
    void autoCreateExportBillNum();
    void selectedBooksForImport(QAction *action);
    void selectedBooksForExport(QAction *action);
    void setCompleterForImport();
    void setCompleterForExport(); // gộp lại không chạy do setCompleter chỉ làm việc với 1 widget
    void updateTotals();
    int  getImportInvoiceId(const QString &numBill);
    int  getExportInvoiceId(const QString &numBill);
    void updatedStockDueImport();
    void updatedStockDueExport();

    void ImportInvoiceDetails(int row);
    void ExportInvoiceDetails(int row);

    void resetImportInvoiceForm();
    void resetExportInvoiceForm();
};

#endif // IMEXPORT_H
