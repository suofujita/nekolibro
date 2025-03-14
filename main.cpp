#include "nekolibro.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NekoLibro w;
    w.show();
    return a.exec();
}
