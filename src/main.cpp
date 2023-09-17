#include "../head/db_additem.h"
#include "../head/database.h"
#include "../head/widget.h"
#include <QApplication>
#include<QDebug>

#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    Widget w;
    w.show();
    return a.exec();
}
