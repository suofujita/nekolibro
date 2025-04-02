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
#include <QStringList>
#include <QStringListModel>
#include <QCompleter>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QStandardItem>
QT_BEGIN_NAMESPACE

class CategoriesWindow;
class ImExport;

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
    QStringList getCategoriesList();
private slots:
    void clickedLogOut();
    void openSalesWindow();
    void openEmployeesWindow();
    void openCategoriesWindow();
    void openImExportWindow();
private:
    Ui::NekoLibro *ui;
    login *pLogin = nullptr;
    SalesWindow *pSaleWindow = nullptr;
    EmployeesWindow *pEmployeesWindow = nullptr;
    CategoriesWindow *pCategoriesWindow = nullptr;
    ImExport *pImExportWindow = nullptr;

};
#endif // NEKOLIBRO_H
