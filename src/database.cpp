#include "../head/database.h"
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

//database::path = "mydatabase.m_db";

// 打开并初始化数据库
QSqlDatabase Database::openAndInit()
{
    QSqlDatabase m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_path);

    if (!m_db.open())
    {
        qDebug() << "Failed to open database:" << m_db.lastError().text();
        QSqlDatabase::removeDatabase(m_db.connectionName());
        return QSqlDatabase();  // 返回一个无效的 QSqlDatabase
    }
    else
    {
        qDebug() << "Database ready";
        // 执行数据库操作（比如初始化表格）
        initTable();
        return m_db;
    }
}
// 初始化表1 question
void Database::initTable()
{
    QSqlQuery query;

    // 获取数据库中所有的表格名称
    QStringList tables = QSqlDatabase::database().tables();

    // 检查所需的表格是否已存在
    if (tables.contains("questions")) {
        qDebug() << "Table 'questions' already exists.";
    } else {
        // 如果表格不存在，创建它
        QString createTableQuery = "CREATE TABLE IF NOT EXISTS questions "
                                   "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "分类 TEXT,"
                                   "小类 TEXT,"
                                   "题目 TEXT,"
                                   "参考 TEXT,"
                                   "次数 INT,"
                                   "总分 INT,"
                                   "平均 DOUBLE)";

        if (!query.exec(createTableQuery)) {
            qDebug() << "Error creating table:" << query.lastError();
        } else {
            qDebug("Table 'questions' init success");
        }
    }
    if (!tables.contains("qstion_info")) {
            // 如果表格不存在，创建它
            QString createQstionInfoTableQuery = "CREATE TABLE IF NOT EXISTS qstion_info "
                                               "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                               "分类 TEXT,"
                                               "小类 TEXT,"
                                               "数量 INT,"
                                               "位置 TEXT)";

            if (!query.exec(createQstionInfoTableQuery)) {
                qDebug() << "Error creating 'qstion_info' table:" << query.lastError();
            } else {
                qDebug() << "Table 'qstion_info' init success";
            }
        }
}

// 添加 一条数据
bool Database::add_one(const QMap<QString, QVariant> &data)
{
    if (!m_db.isOpen()) {
        qDebug() << "数据库未打开。";
        return false;
    }
    QSqlQuery query;
    QString insertSql = "INSERT INTO " + m_tablename + " (";
    QStringList columns;
    QStringList values;

//    // 检查是否缺少必要的键
//    if (!data.contains("分类") || !data.contains("小类") || !data.contains("题目")) {
//        qDebug() << "缺少必要的键。";
//        return false;
//    }
    QStringList same_check_columns;
    same_check_columns <<"id"<<"分类"<<"小类"<<"题目";

    QString same_check_queryCondition = "题目 = '%1'"; // 根据实际情况设置条件
    same_check_queryCondition = same_check_queryCondition.arg(data["题目"].toString());
    QList<QMap<QString, QVariant>> dataList = query_data(same_check_columns, same_check_queryCondition,0);

    if (!dataList.isEmpty()) {
        qDebug() << "Query result:题目 存在";
        for(const QMap<QString, QVariant>check:dataList ){
            if(data["小类"].toString()==check["小类"].toString())
                if(data["分类"].toString()==check["分类"].toString())
                {
                    qDebug() << "Query result: 题目 存在 且分类完全相同 拒绝录入";
                    qDebug()<< "匹配目标 ID  = ["<<check["id"].toInt()<<"]";
                    return false;
                }
        }
    } else {
        qDebug() << "No data found.题目 不存在";
    }

    for (const QString &key : data.keys()) {
        columns << key;
        values << ":" + key;
    }

    insertSql += columns.join(", ") + ") VALUES (";

    for (const QString &key : data.keys()) {
        insertSql += ":" + key + ", ";
        QVariant value = data.value(key);
        if (value.type() == QVariant::Int)
        {
             insertSql.replace(":" + key, QString::number(value.toInt()));
        }
        else if (value.type() == QVariant::Double)
        {
            insertSql.replace(":" + key, QString::number(value.toDouble()));
        }
        else if (value.type() == QVariant::String)
        {
            insertSql.replace(":" + key, "'"+value.toString()+"'");
        } else {
            // 处理其他数据类型（根据需要添加更多类型）
            qDebug() << "Unsupported data type for key:" << key;
            return false;
        }
    }

    insertSql.chop(2); // 移除末尾的 ", "
    insertSql += ")";

    query.prepare(insertSql);

    if (!query.exec()) {
        qDebug() << "Error inserting data:" << query.lastError().text();
        qDebug() << "Error sql [ "<< insertSql <<" ]";
        return false;
    }




    return true;
}
//  删除 一条数据
bool Database::remove_one(const QString & condition)
{
    QSqlQuery query;
    QString deleteSql = "DELETE FROM " + m_tablename + " WHERE " + condition;
    if (!query.exec(deleteSql)) {
        qDebug() << "Error deleting data:" << query.lastError().text();
        return false;
    }
    return true;
}
// 修改 一条数据
bool Database::update_one(const QMap<QString, QVariant> &data, const QString &condition)
{
    QSqlQuery query;
    QString updateSql = "UPDATE " + m_tablename + " SET ";
    QStringList updatePairs;
    for (const QString &key : data.keys())
    {
        updatePairs << key + " = :" + key;
    }
    updateSql += updatePairs.join(", ") + " WHERE " + condition;

    for (const QString &key : data.keys())
    {
        updateSql.replace(":" + key,data.value(key).toString());
//        query.bindValue(":" + key, data.value(key));
    }
    query.prepare(updateSql);
    if (!query.exec())
    {
        qDebug() << "Error updating data:" << query.lastError().text();
        qDebug()<<"sql_update [ "<<updateSql<<" ]";
        return false;
    }
    return true;
}

QList<QMap<QString, QVariant> > Database::query_data(const QStringList &columns, const QString &condition,int mod=0)
{
    QList<QMap<QString, QVariant>> result;
    QSqlQuery query;
    QString select_mod;
    if (mod>0)
        select_mod =" SELECT DISTINCT ";
    else
        select_mod = " SELECT ";
    QString selectSql = select_mod + columns.join(", ") + " FROM " + m_tablename;
    if (!condition.isEmpty()) {
        selectSql += " WHERE " + condition;
    }
     qDebug()<<"sql [ "<<selectSql <<" ]";
    if (!query.exec(selectSql)) {
        qDebug() << "Error querying data:" << query.lastError().text();
        return result;
    }
    while (query.next()) {
        QMap<QString, QVariant> row;
        for (const QString &col : columns) {
            row[col] = query.value(col);
        }
        result.append(row);
    }
    return result;
}

void Database::info_add(const QMap<QString, QVariant> &data)
{
    // 查询 是否出现新情况
    QString condition2 = "小类 = '%1' AND 分类 = '%2'";
    condition2 = condition2.arg(data["小类"].toString()).arg(data["分类"].toString());
    QStringList columns2;
    columns2<<"数量"<<"位置";
    m_tablename="qstion_info";
    QList<QMap<QString, QVariant>> result = query_data(columns2, condition2,0);

    //  统计该情况下 主表中的数量等信息
    QStringList columns3;
    columns3<<"id"<<"分类"<<"小类"<<"题目";
    m_tablename="questions";
    QList<QMap<QString, QVariant>> result2 = query_data(columns3, condition2,0);
    int newid;

    // 查找本函数插入的数据 所在id
    QString cmp1;
    QString cmp2;
    cmp1.append(data["分类"].toString());
    cmp1.append(data["小类"].toString());
    cmp1.append(data["题目"].toString());

    for (const QMap<QString, QVariant> &data2 : result2)
    {
       if(data2["题目"].toString()==data["题目"])
       {
           cmp2.append(data2["分类"].toString());
           cmp2.append(data2["小类"].toString());
           cmp2.append(data2["题目"].toString());
           if(cmp1==cmp2)
               newid = data2["id"].toInt();
       }
    }
    qDebug()<<"new_id = "<<newid;

    if (!result.isEmpty()) {
        qDebug() << "记录存在";
        // 执行  数量 位置 的更新
       // 查询自身数量
            int num = result.at(0)["数量"].toInt();
            QString ids = result.at(0)["位置"].toString();
            if(num>0)
                qDebug()<<"数量存在"<<"num = [ "<<++num<<" ]";
            else
            {
                qDebug()<<"数量不存在";
                num=0;
                // 从主表计算数量
                for (const QMap<QString, QVariant> &data : result2)
                {
                   qDebug() << data;
                    num++;
                }
                qDebug() << "num = [ "<<num<<" ]";
            }
            if(!ids.isEmpty())
            {
                qDebug()<<"位置不空";
                ids.append(',');
                ids.append(QString::number(newid));
                qDebug()<<ids;
            }
            else
            {
                qDebug()<<"位置空";
                int j = 0;
                for (const QMap<QString, QVariant> &data : result2)
                {
                    if(j!=0)
                        ids.append(',');
                   ids.append(QString::number(data["id"].toInt()));
                   j++;
                }
                qDebug()<<ids;
            }
        // 更新数据
        QMap<QString, QVariant> updateData;
        updateData["数量"] = num;
        QString add_ids;
        add_ids.append('"');
        add_ids.append(ids);
        add_ids.append('"');
        updateData["位置"] = add_ids;
//        QString updateCondition = "小类 = '%1'"; // 根据实际情况设置条件
        m_tablename="qstion_info";
//        updateCondition = updateCondition.arg(data["小类"].toString());
        if (update_one(updateData, condition2))
        {
            qDebug() << "小类 Data updated successfully.";
        } else
        {
            qDebug() << "小类 Failed to update data.";
        }
        m_tablename="questions";
    }
    else
    {
        qDebug() << "记录不存在";
        // 插入一条新的记录
        QMap<QString, QVariant> insertData2;
        insertData2["分类"] = data["分类"].toString();
        insertData2["小类"] = data["小类"].toString();
        m_tablename="qstion_info";
        if (add_one(insertData2)) {
            qDebug() << "Data inserted successfully.";
        } else {
            qDebug() << "Failed to insert data.";
        }
        m_tablename="questions";
        info_add(data);
    }
}

void Database::info_remove(const QString &condition)
{
    QStringList columns;
    columns << "id" <<"分类"<<"小类";

    QList<QMap<QString, QVariant>> dataList = query_data(columns, condition,0);
    if (!dataList.isEmpty()) {
        qDebug() << "Query result:";
        //  取出 该分类 该小类 的 数量 和 位置
        m_tablename="qstion_info";
        QStringList columns_info;
        columns_info << "数量" <<"位置";
        QString queryCondition_info = "分类='%1' AND 小类='%2'";
        queryCondition_info = queryCondition_info.arg(dataList.at(0)["分类"].toString()).arg(dataList.at(0)["小类"].toString());
        qDebug()<<"queryCondition_info "<<queryCondition_info;
        QList<QMap<QString, QVariant>> dataList_info = query_data(columns_info, queryCondition_info,0);
        // 数量减一
        int num;
        QString ids;
        QString outputString;
        if (!dataList_info.isEmpty())
        {
            int i =0;
            for(const QMap<QString, QVariant> datainfo:dataList_info)
            {
                qDebug()<<"datainfo"<<datainfo;
                num = datainfo["数量"].toInt();
                i++;
                qDebug()<<"[ "<<i<<"]"<<"num [ "<<num<<" ]";
                ids = datainfo["位置"].toString();
                qDebug()<<"[ "<<i<<"]"<<"ids [ "<<ids<<" ]";
            }
             qDebug()<<"num [ "<<num<<" ]";
            num -=1;
            int del_id = dataList.at(0)["id"].toInt();
            // 位置删除 id
            QString removeValueStr = QString::number(del_id);
            qDebug()<<"ids [ "<<ids<<" ]";
            QStringList parts = ids.split(",");
            QStringList result;

            for (const QString& part : parts) {
                qDebug()<<"part [ "<<part<<" ]& removeValueStr [ "<<removeValueStr<<" ]";
                if (part != removeValueStr) {
                    result.append(part);
                    qDebug()<<"result [ "<<result<<" ]";
                }
            }
            outputString.append("'");
            outputString.append(result.join(","));
            outputString.append("'");
            qDebug() << "outputString."<<outputString;
        }
        else
        {
             qDebug()<<"dataList_info.isEmpty()"<<__LINE__<<__func__;
            return;
        }


        // 更新
        QMap<QString, QVariant> updateData_info;
        updateData_info["数量"] = num;
        updateData_info["位置"] = outputString;
        if (update_one(updateData_info, queryCondition_info)) {
            qDebug() << "info Data updated successfully.";
        } else {
            qDebug() << "Failed to update data."<<__LINE__<<__func__;
        }
        m_tablename="questions";
    } else {
        qDebug() << "No data found."<<__LINE__<<__func__;
    }
}

void Database::change_table(const QString &tablename)
{
    m_tablename = tablename;
}





