#include "mainwindow.h"
#include "dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("La Banque");
    w.show();
    return a.exec();
}
