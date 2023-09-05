#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QList>
#include <QSqlDatabase>

#include <QCoreApplication>
#include <QThreadPool>
#include <QRunnable>
#include <QDebug>
#include <QMutex>

#include <QPushButton>
#include "db_additem.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void widgetInit();
    void PythonInit();
    void setqstion(QSqlDatabase db,int num);// 根据 [id] 设置当前 [问题]
    void onStartClicked();// 点击提交后处理函数
    void workQueue(int i);// py处理队列
    QString getqustion(QSqlDatabase db,int num);// 根据 [id] 获取当前 [问题]
    void click_num_btn(int num);    // 执行点击 自动生成的 命名为 [num] 的按钮
    void showMessageDialog(const QString& title, const QString& text);
    int getmaxItem(QSqlDatabase db); // 获取数据库最大的 [id] 数
    void onShowChild();
    void subGrad();
    int db_get_num_byId(int num,QString item);
    void db_sub_grade(int num,int grade);
    void click_btn(QPushButton* btn);
private:
    Ui::Widget *ui;
    QMap<QString, QPushButton*> buttonMap;
    DB_AddItem* db_AddItem;
};


class Worker : public QObject
{
    Q_OBJECT

public:
    Worker() {}

signals:
    void workFinished();

public slots:
    void startWork(QString qstion,QString anser,int now,QString api);
};

QString send(QString q,QString a,QString api);
#endif // WIDGET_H
