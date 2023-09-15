#include "../head/db_additem.h"
#include "../head/database.h"
#include "../head/widget.h"
#include <QApplication>
#include<QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
