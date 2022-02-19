#include "mainwindow.h"
#include <QApplication>
#include "jsonhandle.h"

JsonHandle * json_obj;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    json_obj = new JsonHandle();

    MainWindow w;
    w.setMinimumSize(600, 800);
    w.show();

    return a.exec();
}
