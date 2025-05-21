#ifndef SALESWINDOW_H
#define SALESWINDOW_H

#include <QDialog>
#include "nekolibro.h"
namespace Ui {
class SalesWindow;
}

class SalesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SalesWindow(QWidget *parent = nullptr);
    ~SalesWindow();
private slots:
    void searchBooks(const QString &text);
    void selectedBooks(QAction *action);
    void cancelBill();
    void saveBill();
    void removeProductFromBill(int row);
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
    void setCompleter();
    int getInvoiceId(const QString &numBill);
};

#endif // SALESWINDOW_H
