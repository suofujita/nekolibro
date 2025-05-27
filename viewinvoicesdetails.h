#ifndef VIEWINVOICESDETAILS_H
#define VIEWINVOICESDETAILS_H

#include <QDialog>

namespace Ui {
class ViewInvoicesDetails;
}

class ViewInvoicesDetails : public QDialog
{
    Q_OBJECT

public:
    explicit ViewInvoicesDetails(QWidget *parent = nullptr);
    ~ViewInvoicesDetails();

private:
    Ui::ViewInvoicesDetails *ui;
};

#endif // VIEWINVOICESDETAILS_H
