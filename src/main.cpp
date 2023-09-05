#include "../head/db_additem.h"
#include "../head/database.h"
#include "../head/widget.h"
#include <QApplication>

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
