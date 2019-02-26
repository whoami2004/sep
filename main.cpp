#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    app.setStyleSheet(TRANSFER("font-family: Courier New; font-size: 24px"));
    app.setFont(QFont(TRANSFER("华文仿宋")));
    app.setOrganizationName(TRANSFER("whoami"));
    app.setOrganizationDomain(TRANSFER("www.topfyf.com/sep/"));
    app.setApplicationName(TRANSFER("sep"));
    app.setApplicationVersion(TRANSFER("1.0"));
    MainWindow *window = new MainWindow;
    window->show();
    return app.exec();
}
