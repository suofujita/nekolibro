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
private:
    Ui::SalesWindow *ui;
    QTimer *time;
    QString currentFullName;
    void updateTotals();
    void showTime();
    void showFullName();
    void moneyReturn();
};

#endif // SALESWINDOW_H
