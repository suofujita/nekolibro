#ifndef CATEGORIESWINDOW_H
#define CATEGORIESWINDOW_H

#include <QDialog>
#include "nekolibro.h"

namespace Ui {
class CategoriesWindow;
}

class CategoriesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CategoriesWindow(QWidget *parent = nullptr);
    ~CategoriesWindow();
private slots:
    void toMainCategories();
    void toAddBook();
    void addBook();
    void removeBook();
private:
    Ui::CategoriesWindow *ui;
    QSqlDatabase db;
    QCompleter *completer = nullptr;
    QStringListModel *model = nullptr;
    NekoLibro *pNekoLibro = nullptr;
    void setupCompleter();

    QStandardItemModel *model1 = nullptr;
    void showData();
    void resetData();


};

#endif // CATEGORIESWINDOW_H
