#ifndef CATEGORIESWINDOW_H
#define CATEGORIESWINDOW_H

#include <QDialog>
#include "nekolibro.h"
#include "saleswindow.h"

namespace Ui {
class CategoriesWindow;
}

class CategoriesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CategoriesWindow(NekoLibro *parent = nullptr);
    ~CategoriesWindow();
public slots:
    void toMainCategories();
    void toAddBook();
    void addBook();
    void removeBook();
    void searchByText();
    void addNewAuthor();
    void addNewCategory();
private:
    Ui::CategoriesWindow *ui;
    QSqlDatabase db;
    QCompleter *completer = nullptr;
    QStringListModel *model = nullptr;
    NekoLibro *pNekoLibro = nullptr;
    SalesWindow *pSalesWindow=nullptr;
    QStandardItemModel *modelForData = nullptr;
    void showData();
    void resetData();
    void loadComboBoxes();
};

#endif // CATEGORIESWINDOW_H
