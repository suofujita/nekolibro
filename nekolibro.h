#ifndef NEKOLIBRO_H
#define NEKOLIBRO_H

#include <QMainWindow>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QString>

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

private:
    Ui::NekoLibro *ui;
};
#endif // NEKOLIBRO_H
