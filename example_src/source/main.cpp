#include <QApplication>

#include "gui/MainWindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
