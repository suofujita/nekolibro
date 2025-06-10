#ifndef EMPLOYEESWINDOW_H
#define EMPLOYEESWINDOW_H

#include <QDialog>
#include "nekolibro.h"

class NekoLibro;
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
    QSqlDatabase db;
    void allTablesUI();
    void loadUsersInfor();
    void loadLast6MonthPerUser();
    void loadThisMonthData();
};

#endif // EMPLOYEESWINDOW_H
