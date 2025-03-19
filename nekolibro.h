#ifndef NEKOLIBRO_H
#define NEKOLIBRO_H

#include <QMainWindow>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QString>
#include <QTimer>
#include "database.h"
#include "login.h"
#include "saleswindow.h"
#include "employeeswindow.h"
#include "categorieswindow.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class NekoLibro;
}
QT_END_NAMESPACE

class NekoLibro : public QMainWindow
{
    Q_OBJECT

public:
    NekoLibro(QWidget *parent = nullptr);
    ~NekoLibro();
private slots:
    void clickedLogOut();
    void openSalesWindow();
    void openEmployeesWindow();
    void openCategoriesWindow();
private:
    Ui::NekoLibro *ui;
    login *pLogin = nullptr;
    SalesWindow *pSaleWindow = nullptr;
    EmployeesWindow *pEmployeesWindow = nullptr;
    CategoriesWindow *pCategoriesWindow = nullptr;
};
#endif // NEKOLIBRO_H
