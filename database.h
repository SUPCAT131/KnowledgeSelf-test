#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>

class database
{
public:
    database() {}
    QSqlDatabase opendb(const QString &name);
    void initdb(const QString &name);  // 初始化数据库
    QList<QString> DBSearch(QSqlDatabase db,QString fahers="分类");                  // 数据库 根据字段名 查询 并返回列表
    QList<QString> DBSearch(QSqlDatabase db,QString fathername,QString fathers="分类",QString children="小类");// 数据库 根据字段名 条件 查询 并返回列表
    void initTable();
    bool add_db(QSqlDatabase db,QString category, QString subCategory, QString question, QString answer);
    QList<int> DBSelect(QSqlDatabase db,int max,QString fathername="无",QString childname="无");
};

#endif // DATABASE_H
