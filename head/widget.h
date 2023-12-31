﻿#ifndef WIDGET_H
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
#include <QFile>
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
    void click_btn(QPushButton* btn);
    void showMessageDialog(const QString& title, const QString& text);
    void test();
    void grade_btn_clicked(int grade);
    void creat_local_file();    // 用于生成本地答题记录
    void creat_local_think(QString str);
    QString textSplitLine(const QString& text);
    QString creat_file(QString dirPath,QString tail,QString filename);
signals:
    void send_myAnswer(QList<int>select_list,QList<QString>answer_list,QString api);
    void progress_hide();
    void send_fixAnserStr(QString);
    void clear_data();
public slots:
    void get_reslut_l(QStringList reslt_l);
    void get_waitTime(int time);
    void get_fixAnser(QString str);
private:
    Ui::Widget *ui;
    QMap<QString, QPushButton*> buttonMap;
    QList<int> m_selectlist;
    QList<int> m_grade_l;
    QList<QString> m_qstion_l;
    QList<QString> m_answer_l;
    QList<QString> m_result_l;
    QList<QString> m_fixAnswer_l;
    int m_lineTextMax;
    float m_lineMaxFix;
    int m_printGrade;
    QList<QMap<QString, QVariant>> m_data;
    void select_items(int select_num);  // model 1 从数据库按比例抽题（表 id 值） 并 存储于私有属性 m_selectlist 中
};
#endif // WIDGET_H
