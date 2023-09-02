#include <Python.h>
#include <iostream>
#include <windows.h>
#include "widget.h"
#include <QDebug>
#include <QApplication>
#include <QString>
#include "pythreadstatelock.h"
using namespace std;

void Widget::PythonInit()
{
//    QString path1 = QCoreApplication::applicationDirPath()+"/python311/";
//    Py_setPythonHome((wchar_t*)(reinterpret_cast<const wchar_t*>(path1.utf16())));

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
            PyRun_SimpleString("sys.path.append('./')");

//            // 初始化线程支持
//            PyEval_InitThreads();
//            // 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
//            PyEval_ReleaseThread(PyThreadState_Get());
            qDebug("初始化Python解释器成功!");
        }
    }
}

QString send(QString q,QString a,QString api){
    PyEval_ReleaseThread(PyThreadState_Get());
    PyThreadStateLock PythreadLock;   // 获取全局锁

    //加载模块，模块名称为scirptThree，就是scirptThree.py文件
    PyObject *pModule = PyImport_ImportModule("testpython");

    //如果加载失败，则返回
    if ( pModule==nullptr )
    {
        qCritical()<<"加载Python文件失败";
        PyErr_Print();
        return QString();
    }
    qDebug()<<"加载Python文件成功";
    //通过PyObject_CallMethod方法向加载的pModule模块对象的receiveArg方法注入参数。sss表示String String String;char_q,char_c,char_a即为传入的参数
    QString c;
    // c 为 根据问题查询 库中 对应的 答案 -doing
    std::string q_s = q.toStdString();  // 问题
    std::string q_a = a.toStdString();  // 我的回答
    std::string q_c = c.toStdString();  // 库中的参考回答
    std::string q_api = api.toStdString();
    char* char_q = const_cast<char*>(q_s.c_str());
    char* char_a = const_cast<char*>(q_a.c_str());
    char* char_c = const_cast<char*>(q_c.c_str());
    char* char_api = const_cast<char*>(q_api.c_str());
    qDebug()<<"问题是:["<<char_q<<"] 我的参考是:["<<char_c<<"]我的回答是:["<<char_a<<"]请问我的参考和我的回答是否全面";
    PyObject* pRet=PyObject_CallMethod(pModule,"check_reslt","ssss",char_q,char_c,char_a,char_api);
    if (pRet != nullptr) {
        PyObject *py_str = PyObject_Str(pRet); // 获取返回值
        if (PyUnicode_Check(py_str)) {
            QString qstr = QString::fromUtf8(PyUnicode_AsUTF8(py_str));
            // 使用QString qstr接收
            Py_DECREF(py_str); // 释放 Python 对象
            Py_DECREF(pRet);   // 释放方法返回值对象
            return qstr;
        } else {
            // 返回值不是字符串
            PyErr_Print();
            PyErr_Clear(); // 清理 Python 异常
            Py_DECREF(pRet);   // 释放方法返回值对象
            QString qstr = "获取失败";
            return qstr;
        }
    } else {
        // 方法调用失败
        PyErr_Print();
        PyErr_Clear(); // 清理 Python 异常
        QString qstr = "调用方法失败";
        return qstr;
    }
}

