#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QThread>

class MyThread : public QThread
{
    Q_OBJECT // 这是信号和槽机制所需的宏

public:
    explicit MyThread(QObject *parent = nullptr);
    void doWork();
signals:
    // 声明你的自定义信号
    void send_result_list(QStringList result_list);
    void send_part_result_list(QStringList result_list);
    void send_result(QString str);
    void startWork();
    void workFinished();
    void send_waitTime(int);
    void send_progressBar(int);
public slots:
    // 声明你的自定义槽函数
    void set_intList_strList(QList<int>intList,QList<QString>strList,QString api);

private:
    QList<int> m_get_select_list;
    QList<QString> m_get_answer_list;
    QList<QString> m_result_list;
    QString m_api;
protected:
    void run() override;
};

#endif // MYTHREAD_H
