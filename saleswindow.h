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
    //void saveBill();
private:
    Ui::SalesWindow *ui;
    QTimer *time;
    double totalPrice = 0.0 ;
    QString currentFullName;
    void updateTotals();
    void showTime();
    void showFullName();
    void moneyReturn(const QString &text);
    void autoCreateNumBill();   // tạo mã số hóa đơn theo định dạng NekoLibro dd-mm-yyyy
};

#endif // SALESWINDOW_H
