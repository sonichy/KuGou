#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qSetMessagePattern("[ %{file}: %{line} ] %{message}");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}