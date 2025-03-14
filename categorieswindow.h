#ifndef CATEGORIESWINDOW_H
#define CATEGORIESWINDOW_H

#include <QDialog>

namespace Ui {
class CategoriesWindow;
}

class CategoriesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CategoriesWindow(QWidget *parent = nullptr);
    ~CategoriesWindow();

private:
    Ui::CategoriesWindow *ui;
};

#endif // CATEGORIESWINDOW_H
