#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <QTextCodec>

using namespace std;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    //QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

    main_window w;
    w.show();

    return a.exec();
}
