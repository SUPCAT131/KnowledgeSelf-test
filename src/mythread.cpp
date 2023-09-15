#include "../head/mythread.h"
#include "../head/pythreadstatelock.h"
#include "../head/database.h"
#include <Python.h>

#include <iostream>
#include <windows.h>
#include <QDebug>
#include <QApplication>
#include <QString>
#include <QElapsedTimer>

MyThread::MyThread(QObject *parent)
{
}

void MyThread::run()
{

    qDebug()<<"thread start [ 线程启动 ]";
    qDebug()<<"子线程地址"<<QThread::currentThread();
    qDebug()<<"[ thread working 线程正在工作 ]";
    if (!Py_IsInitialized())
        {
            //1.初始化Python解释器，这是调用操作的第一步
            Py_Initialize();
            if (!Py_IsInitialized())
                qDebug("初始化Python解释器失败!");
            else
            {
                //执行单句Python语句，用于给出调用模块的路径，否则将无法找到相应的调用模块
                PyRun_SimpleString("import sys");
                PyRun_SimpleString("sys.argv = ['python.py']");
                PyRun_SimpleString("sys.path.append('./py')");
                PyRun_SimpleString("print(sys.path)"); // 打印 sys.path
                qDebug("初始化Python解释器成功!");
            }
        }
    PyObject *pModule = PyImport_ImportModule("testpython");
    //如果加载失败，则返回
        if ( pModule==nullptr )
        {
            QString str = "加载Python文件失败";
            qCritical()<<str;
            PyErr_Print();
            emit send_result(str);
            return ;
        }
        qDebug()<<"加载Python文件成功";
        //通过PyObject_CallMethod方法向加载的pModule模块对象的receiveArg方法注入参数。sss表示String String String;char_q,char_c,char_a即为传入的参数
        // model1 从数据库获取需要的值
        Database myDb("mydatabase.db","questions");
        QStringList columns;
        columns << "id" <<"分类"<<"题目"<<"参考";
        // model1 end
        int index = 0;
        QElapsedTimer timer;

        for(const int i : m_get_select_list)
        {
            timer.start(); // 启动计时器
            QString queryCondition = "id = %1"; // 根据实际情况设置条件
            queryCondition = queryCondition.arg(i);
            QList<QMap<QString, QVariant>> dataList = myDb.query_data(columns, queryCondition,0);
            if (!dataList.isEmpty())
            {
                qDebug() << "子线程从数据库查询[ id ] ="<<i<<" 的数据成功";
                for (const QMap<QString, QVariant> &data : dataList)
                {
                    QString q = data["题目"].toString();
                    QString a = m_get_answer_list.at(index);
                    QString c = data["参考"].toString();

                    std::string q_s = q.toStdString();  // 问题
                    std::string q_a = a.toStdString();  // 我的回答
                    std::string q_c = c.toStdString();  // 库中的参考回答
                    std::string q_api = m_api.toStdString();
                    qDebug()<<"1 -- 问题是:["<<q<<"] 我的参考是:["<<c<<"]我的回答是:["<<a<<"]请问我的参考和我的回答是否全面";
                    char* char_q = const_cast<char*>(q_s.c_str());
                    char* char_a = const_cast<char*>(q_a.c_str());
                    char* char_c = const_cast<char*>(q_c.c_str());
                    char* char_api = const_cast<char*>(q_api.c_str());
                    qDebug()<<"2 -- 问题是:["<<char_q<<"] 我的参考是:["<<char_c<<"]我的回答是:["<<char_a<<"]请问我的参考和我的回答是否全面";
                    PyObject* pRet=PyObject_CallMethod(pModule,"check_reslt","ssss",char_q,char_c,char_a,char_api);
                    QString qstr;
                    if (pRet != nullptr) {
                        PyObject *py_str = PyObject_Str(pRet); // 获取返回值
                        if (PyUnicode_Check(py_str))
                        {
                             qstr = QString::fromUtf8(PyUnicode_AsUTF8(py_str));
                            // 使用QString qstr接收
                            Py_DECREF(py_str); // 释放 Python 对象
                            Py_DECREF(pRet);   // 释放方法返回值对象
                        }
                        else
                        {
                            // 返回值不是字符串
                            PyErr_Print();
                            PyErr_Clear(); // 清理 Python 异常
                            Py_DECREF(pRet);   // 释放方法返回值对象
                            qstr = "获取失败";
                        }
                    }
                    else
                    {
                        // 方法调用失败
                        PyErr_Print();
                        PyErr_Clear(); // 清理 Python 异常
                        qstr = "调用方法失败";
                    }
                    m_result_list.append(qstr);
                }
            }
            else
            {
                qDebug() << "No data found."<<__LINE__<<__func__;
            }
            index++;
            emit send_progressBar(index);
            if(index%3==0&&index!=0&&index<m_get_select_list.size())
            {
                QStringList stringList = m_result_list;
                emit send_part_result_list(stringList);
                qint64 elapsed = timer.elapsed();
                qDebug() << "函数执行时间：" << elapsed << "毫秒";
                int waitTime =static_cast<int>((60000 - elapsed)/1000) +1;
                qDebug() << "send_waitTime" << waitTime;
                emit send_waitTime(waitTime);
                QThread::msleep(61*1000 - elapsed);
                qDebug() << "sleep" << 61*1000 - elapsed;
                timer.invalidate(); // 停止计时器
            }

        }
//    emit workFinished();
    qDebug()<<"[ thread finished work 线程工作结束 ]";
    QStringList stringList = m_result_list;
    emit send_result_list(stringList);
}

void MyThread::set_intList_strList(QList<int> intList, QList<QString> strList,QString api)
{
    m_get_answer_list.clear();
    m_get_select_list.clear();
    m_api.clear();

    m_get_select_list = intList;
    m_get_answer_list=strList;
    m_api =api;
    qDebug()<<"m_get_select_list"<<m_get_select_list;
    qDebug()<<"m_get_answer_list"<<m_get_answer_list;
    qDebug()<<"m_api"<<m_api;
    emit startWork();
}


