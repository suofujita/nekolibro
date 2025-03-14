#ifndef EMPLOYEESWINDOW_H
#define EMPLOYEESWINDOW_H

#include <QDialog>

namespace Ui {
class EmployeesWindow;
}

class EmployeesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeesWindow(QWidget *parent = nullptr);
    ~EmployeesWindow();

private:
    Ui::EmployeesWindow *ui;
};

#endif // EMPLOYEESWINDOW_H
