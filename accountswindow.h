#ifndef ACCOUNTSWINDOW_H
#define ACCOUNTSWINDOW_H

#include <QWidget>

namespace Ui {
class AccountsWindow;
}

class AccountsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsWindow(QWidget *parent = nullptr);
    ~AccountsWindow();

private:
    Ui::AccountsWindow *ui;
};

#endif // ACCOUNTSWINDOW_H
