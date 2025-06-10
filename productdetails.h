#ifndef PRODUCTDETAILS_H
#define PRODUCTDETAILS_H

#include <QDialog>
#include "nekolibro.h"
#include "saleswindow.h"

namespace Ui {
class ProductDetails;
}

class ProductDetails : public QDialog
{
    Q_OBJECT

public:
    explicit ProductDetails(QWidget *parent = nullptr);
    ~ProductDetails();
private slots:
    void searchByText();
    void onProductClicked(const QModelIndex &index);
signals:
    void productSelected(QString id, QString title, QString author, double price, int stock);
private:
    Ui::ProductDetails *ui;
    QStandardItemModel *modelForData = nullptr;
    void showData();

};

#endif // PRODUCTDETAILS_H
