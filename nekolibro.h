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
#include "employeeswindow.h"
#include <QStringList>
#include <QStringListModel>
#include <QCompleter>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QMenu>
#include <QToolButton>
#include <QAction>
#include <QDate>
#include <QLabel>
#include <QTimer>
#include <QScreen>
#include <QSpinBox>
QT_BEGIN_NAMESPACE

class CategoriesWindow;
class SalesWindow;
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
    QString currentUser, currentFullName;
    void setCurrentUser(QString username);
    void showUserName();
    void showFullName();
private slots:
    void clickedLogOut();
    void openSalesWindow();
    void openEmployeesWindow();
    void openCategoriesWindow();
    void openImExportWindow();
    void gotoAddBook();
private:
    Ui::NekoLibro *ui;
    login *pLogin = nullptr;
    SalesWindow *pSaleWindow = nullptr;
    EmployeesWindow *pEmployeesWindow = nullptr;
    CategoriesWindow *pCategoriesWindow = nullptr;
    ImExport *pImExportWindow = nullptr;
    QSqlDatabase db;
    QMenu *menuCategories = nullptr;
    QTimer *time;

    void showTime();

    QString getCurrentUser();
};
#endif // NEKOLIBRO_H
