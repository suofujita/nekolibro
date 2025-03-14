#include "nekolibro.h"
#include "./ui_nekolibro.h"

NekoLibro::NekoLibro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NekoLibro)
{
    ui->setupUi(this);
}

NekoLibro::~NekoLibro()
{
    delete ui;
}
