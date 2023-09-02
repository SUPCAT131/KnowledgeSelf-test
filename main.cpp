#include "widget.h"

#include <QApplication>
#include "database.h"
#include "db_additem.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    QString path = "mydatabase.db";
    database db;
    db.initdb(path);
//    DB_AddItem b;
//    b.show();
    w.show();
    return a.exec();
}
