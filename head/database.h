#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QVariant>

class Database
{
public:
    Database(QString path,QString name)
    {
        m_path = path;
        m_tablename = name;
        m_db = openAndInit();
    }
    ~Database(){
        // 析构函数中关闭数据库连接
        m_db.close();
    };
    QSqlDatabase openAndInit();
    void initTable();                                 // 初始化表格
    // 增
    bool add_one(const QMap<QString, QVariant> &data);
    // 删
    bool remove_one(const QString& condition);
    // 改
    bool update_one(const QMap<QString, QVariant> &data, const QString &condition);
    // 查
    QList<QMap<QString, QVariant>>query_data(const QStringList &columns, const QString &condition,int mod);
    // 主表增 辅表应进行的操作
    void info_add(const QMap<QString, QVariant> &data);
    // info辅表 删
    void info_remove(const QString& condition);
    void change_table(const QString& tablename);
private:
    QSqlDatabase m_db;
    QString m_path;
    QString m_tablename;
};

#endif // DATABASE_H
