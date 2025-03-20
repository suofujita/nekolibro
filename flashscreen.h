#ifndef FLASHSCREEN_H
#define FLASHSCREEN_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class FlashScreen;
}

class FlashScreen : public QWidget
{
    Q_OBJECT

public:
    explicit FlashScreen(QWidget *parent = nullptr);
    ~FlashScreen();
signals:
    void ScreenFinished();
private:
    Ui::FlashScreen *ui;
};

#endif // FLASHSCREEN_H
