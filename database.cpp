#include "database.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QApplication>


#include <QCoreApplication>
#include <QSqlRecord>
#include <QList>
#include <QMap>
#include <QTime>
#include <cstdlib>

//database::path = "mydatabase.db";

QSqlDatabase database::opendb(const QString &name)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(name);

    if(!db.open())
    {
        qDebug()<<db.lastError();
        qFatal("failed to connect.");
        QSqlDatabase::removeDatabase(db.connectionName());
        return QSqlDatabase();  // 返回一个无效的 QSqlDatabase
    }
    else
    {
        return db;
    }
}
void database::initdb(const QString &name)
{

//    QString path = "mydatabase.db";
    QSqlDatabase db = opendb(name);
    if (db.isValid()) {
        qDebug() << "Database connection is valid.";
        // 执行数据库操作
        initTable();
    } else {
        qDebug() << "Database connection is invalid.";
        // 处理无效连接
    }
    db.close();
}
void database::initTable()
{
    QSqlQuery query;

    // 创建一个名为 "questions" 的表格
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS questions "
                               "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "分类 TEXT,"
                               "小类 TEXT,"
                               "题目 TEXT,"
                               "参考答案 TEXT,"
                               "出题次数 INT,"
                               "总分数 INT,"
                               "平均分 DOUBLE)";

    if (!query.exec(createTableQuery)) {
        qDebug() << "Error creating table:" << query.lastError();
    }
}
QList<QString> database::DBSearch(QSqlDatabase db,QString fahers)
{
    QList<QString> getList;

    // 检查数据库连接是否有效
    if (!db.isValid()) {
        qDebug() << "获取到无效的数据库连接。";
        qDebug() << "函数：" << Q_FUNC_INFO << "行号：" << __LINE__;
        return getList; // 返回空列表
    }

    // 执行 SQL 查询以从指定字段中检索不重复的值
    QString queryString = QString("SELECT DISTINCT %1 FROM questions").arg(fahers); // 替换为表名
    QSqlQuery query(queryString, db);

    if (!query.exec()) {
        qDebug() << query.lastError();
        qFatal("查询执行失败。");
    }

    // 获取并添加结果到列表中
    while (query.next()) {
        QString value = query.value(0).toString();
        getList.append(value);
    }

    return getList;}

QList<QString> database::DBSearch(QSqlDatabase db,QString fathername,QString fathers,QString children)
{

    QList<QString> getList;

    // 检查数据库连接是否有效
    if (!db.isValid()) {
        qDebug() << "无效的数据库连接。";
        qDebug() << "函数：" << Q_FUNC_INFO << "行号：" << __LINE__;
        return getList; // 返回空列表
    }

    // 执行 SQL 查询以从指定大类下检索不重复的小类
    QString queryString = QString("SELECT DISTINCT %1 FROM questions WHERE %2 = '%3'").arg(children).arg(fathers).arg(fathername);
    QSqlQuery query(queryString, db);
//    query.bindValue(":name", fathername);

    if (!query.exec()) {
        qDebug() << "queryString:" << queryString;
        qDebug() << query.lastError();
        qDebug() << "查询执行失败。";
        return getList; // 返回空列表
    }

    // 获取并添加结果到列表中
    while (query.next()) {
        QString value = query.value(0).toString();
        getList.append(value);
    }

    return getList;
}
bool database::add_db(QSqlDatabase db,QString category, QString subCategory, QString question, QString answer)
{
    if (!db.isOpen()) {
        qDebug() << "数据库未打开。";
        return false;
    }


    QString queryString = "SELECT COUNT(*) FROM questions WHERE 分类 = :category AND 小类 = :subcategory AND 题目 = :question";
    QSqlQuery query(db);
    query.prepare(queryString);
    query.bindValue(":category", category);
    query.bindValue(":subcategory", subCategory);
    query.bindValue(":question", question);
    if (!query.exec())
    {
        qDebug() << query.lastError();
        qDebug() << "查询执行失败：" << query.lastError();
        return false;
    }

    if (query.next()) {
       int count = query.value(0).toInt();
       if(count > 0)
       {
           qDebug() << "数据已存在";
           return false; // 如果存在记录，返回 true，否则返回 false
       }
    }

    query.prepare("INSERT INTO questions (分类, 小类, 题目, 参考答案) "
                  "VALUES (:category, :subCategory, :question, :answer)");
    query.bindValue(":category", category);
    query.bindValue(":subCategory", subCategory);
    query.bindValue(":question", question);
    query.bindValue(":answer", answer);

    if (!query.exec()) {
        qDebug() << query.lastError();
        qDebug() << "插入数据失败。";
        return false;
    }

    return true;
}
//    int fathermax;
//    int childmax;
    // 如果fathername!=无 fathermax =1 ;
    // 否则 fathermax = 全部大类数
    // 如果 fathermax=1 childmax = 该大类下的所有小类数
    // 否则 childmax = 全部小类数
    // 如果 max > childmax 但 < 2child max

QList<int> database::DBSelect(QSqlDatabase db, int max, QString fathername, QString childname)
{
    QList<int> selectedIds;
    if (!db.open()) {
        qDebug() << "无法连接到数据库。";
        return selectedIds;
    }

    // 查询数据库以获取各个分类和它们的数据数量
    QString queryString = "SELECT id FROM questions WHERE 1 = 1"; // WHERE 1 = 1 用于后续的条件拼接
    if (fathername != "无") {
        queryString += QString(" AND 分类 = '%1'").arg(fathername);
    }
    if (childname != "无") {
        queryString += QString(" AND 小类 = '%1'").arg(childname);
    }

    // 执行查询语句
    QSqlQuery idQuery(queryString);
    while (idQuery.next()) {
        selectedIds.append(idQuery.value(0).toInt());
    }

    // 如果需要 max 条数据，打乱所选 ID 列表并截取前 max 个
    if (max > 0 && max <= selectedIds.size()) {
        std::random_shuffle(selectedIds.begin(), selectedIds.end());
        selectedIds = selectedIds.mid(0, max);
    }

    db.close();
    qDebug() << "所选的 ID：" << selectedIds;
    return selectedIds;
}

