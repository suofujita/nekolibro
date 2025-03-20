#include "flashscreen.h"
#include "ui_flashscreen.h"

FlashScreen::FlashScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FlashScreen)
{
    ui->setupUi(this);

    /* Ẩn thanh tiêu đề và làm trong suốt background*/
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QTimer::singleShot(3000,this,[this](){
        emit ScreenFinished();
        close();
    });
}

FlashScreen::~FlashScreen()
{
    delete ui;
}
