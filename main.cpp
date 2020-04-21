#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.setAttribute(Qt::WA_DeleteOnClose, true);
    w.show();
    //w.serial_set_init();
    //w.serial_set_default();

    return a.exec();
}
