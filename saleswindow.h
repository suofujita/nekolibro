#ifndef SALESWINDOW_H
#define SALESWINDOW_H

#include <QDialog>
#include "nekolibro.h"
#include "categorieswindow.h"
namespace Ui {
class SalesWindow;
}

class SalesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SalesWindow(QWidget *parent = nullptr);
    ~SalesWindow();
    void setCompleter();
private slots:
    void searchBooks(const QString &text);
    void selectedBooks(QAction *action);
    void cancelBill();
    void saveBill();
    void removeProductFromBill();
    void loadStock(int row, int col);
private:
    Ui::SalesWindow *ui;

    QTimer *time;
    int totalQuantity = 0;
    double totalPrice = 0.0;
    QString currentFullName;
    int currentUserId;

    void updateTotals();
    void showTime();
    void showFullName();
    void moneyReturn(const QString &text);
    void autoCreateBillNum();   // tạo mã số hóa đơn theo định dạng NekoLibro-GDddmmyyyy-xxxx

    int getInvoiceId(const QString &numBill);
    int getStock(int row);
};

#endif // SALESWINDOW_H
