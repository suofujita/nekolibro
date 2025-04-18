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
    explicit CategoriesWindow(NekoLibro *parent = nullptr);
    ~CategoriesWindow();
public slots:
    void toMainCategories();
    void toAddBook();
    void addBook();
    void removeBook();
    void searchByText();
private:
    Ui::CategoriesWindow *ui;
    QSqlDatabase db;
    QCompleter *completer = nullptr;
    QStringListModel *model = nullptr;
    NekoLibro *pNekoLibro = nullptr;
    void setupCompleter();

    QStandardItemModel *modelForData = nullptr;
    void showData();
    void resetData();
};

#endif // CATEGORIESWINDOW_H
