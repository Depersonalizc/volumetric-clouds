//#include "mainwindow.h"

//#include <QApplication>
//#include <QScreen>
//#include <iostream>
//#include <QSettings>


//int main(int argc, char *argv[]) {
//    QApplication a(argc, argv);

//    QCoreApplication::setApplicationName("Volumetric Clouds Rendering!");
//    QCoreApplication::setOrganizationName("CS 1230");
//    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

//    QSurfaceFormat fmt;
//    fmt.setVersion(4, 1);
//    fmt.setProfile(QSurfaceFormat::CoreProfile);
//    QSurfaceFormat::setDefaultFormat(fmt);

//    MainWindow w;
//    w.initialize();
//    w.resize(1200, 600);
//    int desktopArea = QGuiApplication::primaryScreen()->size().width() *
//                      QGuiApplication::primaryScreen()->size().height();
//    int widgetArea = w.width() * w.height();
//    if (((float)widgetArea / (float)desktopArea) < 0.75f) {
//        w.show();
//    } else {
//        w.showMaximized();
//    }

//    int return_val = a.exec();
//    w.finish();


//    return return_val;
//}
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
