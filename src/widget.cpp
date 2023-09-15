#include "../head/db_additem.h"
#include "../head/database.h"
#include "../head/widget.h"
#include "../head/mythread.h"
#include "ui_widget.h"
#include <QtDebug>
#include <QScrollArea>
#include <QSqlQuery>
#include <QThread>
#include <QMouseEvent>
#include <QMessageBox>
#include <QSqlError>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QElapsedTimer>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    qDebug()<<"主线程地址"<<QThread::currentThread();
    widgetInit();
//    connect(ui->pB_sub, &QPushButton::clicked, this, &Widget::onStartClicked);
    connect(ui->pB_start, &QPushButton::clicked, [=]() {
        int select_num = ui->spinBox->value(); // 获取用户输入的按钮数量
        m_selectlist.clear();
        // model 1 从数据库按比例抽题（表 id 值） 并 存储于私有属性 m_selectlist 中
        select_items(select_num);
       // model 1 ---end
        // model 2 根据抽题列表 从数据库下载每题的信息 存入 m_data 中
        Database myDb("mydatabase.db","questions");
        QStringList columns;
        columns << "id" <<"分类"<<"小类"<<"题目";
        for (int i : m_selectlist)
        {
            QString queryCondition = "id =%1";
            queryCondition = queryCondition.arg(i);
            QList<QMap<QString, QVariant>> dataList = myDb.query_data(columns, queryCondition,0);
            m_data.append(dataList[0]);
        }
         // model 2 end
        qDebug()<<"m_data"<<m_data;
        // 清空 widget_qstion 中的内容
        QLayout *qstionLayout = ui->widget_qstion->layout();
        if(qstionLayout!=nullptr)
        {
            QLayoutItem *child;
            while ((child = qstionLayout->takeAt(0)) != nullptr) {
                if (QWidget *widget = child->widget())
                {
                    widget->setParent(nullptr);
                }
                delete child; // 删除布局项
            }
            delete qstionLayout;
        }

        QGridLayout *newLayout = new QGridLayout(ui->widget_qstion);

        for (int i = 0; i < select_num; ++i) {
            QPushButton *button = new QPushButton(QString::number(i + 1));
            int row = i / 2; // 在每行最多显示2个按钮，计算所在行数
            int column = i % 2; // 在每行中的列数

            button->setMinimumSize(40,40); // 设置按钮的最小大小为40*40像素
            button->setMaximumSize(50,50);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // 设置按钮的大小策略

            newLayout->addWidget(button, row, column);
            buttonMap.insert(button->text(), button);
            // 连接按钮点击事件
            QObject::connect(button, &QPushButton::clicked, [=]() {
                ui->pB_save_qstion->setEnabled(true);

                ui->lb_fthrname->setText(m_data[i]["分类"].toString());
                ui->lb_childname->setText(m_data[i]["小类"].toString());
                ui->lb_qstion->setText(m_data[i]["题目"].toString());
                // model2 end

                ui->textEdit->clear();
                ui->textBrowser->clear();
                ui->lb_num->setNum(i+1);
                if(m_answer_l.size()>i)
                {
                    ui->textEdit->setText(m_answer_l[i]);

                }
                if(m_result_l.size()>i)
                {
                    ui->textBrowser->setText(m_result_l[i]);
                }
                ui->pB_0->setStyleSheet("");
                ui->pB_30->setStyleSheet("");
                ui->pB_60->setStyleSheet("");
                ui->pB_90->setStyleSheet("");

//                ui->pB_0->setEnabled(true);
//                ui->pB_30->setEnabled(true);
//                ui->pB_60->setEnabled(true);
//                ui->pB_90->setEnabled(true);

                QString style = {"background-color:rgb(26, 188, 156);"
                                  "border-radius: 5px;"
                                  "border-width: 1px;"
//                                    "color: white; "
                                  "border-style: outset;"
                                  "border-color:rgb(26, 188, 156);}"
                                  "QPushButton:hover{background-color:rgb(52, 152, 219);}"
                                  "QPushButton:pressed{background-color:rgb(155, 89, 182);}"};
                if(m_grade_l.size()>i)
                    switch (m_grade_l.at(i))
                    {
                     case 1:
                        qDebug("case 1");
                        ui->pB_0->setStyleSheet(style);
                        break;
                    case 2:
                       ui->pB_30->setStyleSheet(style);
                       break;
                    case 3:
                       ui->pB_60->setStyleSheet(style);
                       break;
                    case 4:
                       ui->pB_90->setStyleSheet(style);
                       break;
                    }
                if(i==0)
                    ui->pB_last->setEnabled(false);
                else
                    ui->pB_last->setEnabled(true);
                if(i==select_num-1)
                    ui->pB_next->setEnabled(false);
                else
                    ui->pB_next->setEnabled(true);
            });
        }

        QWidget *scrollWidget = new QWidget(); // 创建一个新的 QWidget 作为滚动区域的内容
        scrollWidget->setLayout(newLayout);

        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true); // 设置滚动区域的大小可调整
        scrollArea->setWidget(scrollWidget); // 将滚动区域的内容设置为 scrollWidget

        QVBoxLayout *mainLayout = new QVBoxLayout(ui->widget_qstion);
        mainLayout->addWidget(scrollArea); // 将滚动区域添加到主布局中


        ui->widget_qstion->setLayout(newLayout);

        ui->spinBox->setEnabled(false);
        ui->pB_start->setEnabled(false);

    });
    connect(ui->pB_save_qstion,&QPushButton::clicked,this,[=](){
            int now = ui->lb_num->text().toInt();
            int now_a = m_answer_l.size();
            int now_r = m_result_l.size();
            QString anser = ui->textEdit->toPlainText();
            qDebug()<<"now="<<now<<"now_r"<<now_r<<"now_a"<<now_a;
            while(now_a<now)
            {
                m_answer_l.append("");
                now_a = m_answer_l.size();
                qDebug()<<"now="<<now<<"now_a"<<now_a<<"m_answer_l: "<<m_answer_l;
            }
            while (now_r<now)
            {
                m_result_l.append("");
                now_r = m_result_l.size();
                qDebug()<<"now="<<now<<"now_a"<<now_r<<"m_result_l: "<<m_result_l;
            }
            m_answer_l[now-1]=anser;
            qDebug()<<"now="<<now<<"now_a"<<now_a<<"m_answer_l: "<<m_answer_l<<"now_r"<<now_r<<"m_result_l: "<<m_result_l;
            QPushButton *targetButton = buttonMap.value(ui->lb_num->text());
            targetButton->setStyleSheet("QPushButton{"
                                        "background-color:rgb(26, 188, 156);"
                                        "border-radius: 5px;"
                                        "border-width: 1px;"
    //                                    "color: white; "
                                        "border-style: outset;"
                                        "border-color:rgb(26, 188, 156);}"
                                        "QPushButton:hover{background-color:rgb(52, 152, 219);}"
                                        "QPushButton:pressed{background-color:rgb(155, 89, 182);}"); // 修改按钮的样式
            if(ui->textEdit->toPlainText().isEmpty())
                targetButton->setStyleSheet(""); // 修改按钮的样式
            if(ui->lb_num->text().toInt()==ui->spinBox->value())
                ui->pB_sub->setEnabled(true);
    });
    connect(ui->pB_last,&QPushButton::clicked,[=](){
        int now = ui->lb_num->text().toInt()-1;
        QPushButton *targetButton = buttonMap.value(QString::number(now));
        click_btn(targetButton);
    });
    connect(ui->pB_next,&QPushButton::clicked,[=](){
        int now = ui->lb_num->text().toInt()+1;
        QPushButton *targetButton = buttonMap.value(QString::number(now));
        click_btn(targetButton);
    });
    connect(ui->pB_addItem,&QPushButton::clicked,[=](){
        DB_AddItem *db_AddItem = new DB_AddItem(nullptr);
        db_AddItem->show();
        showMessageDialog("提示","注意 添加成功后会[重置]答题界面");
        connect(db_AddItem,&DB_AddItem::refalshWidgtInit,this,&Widget::widgetInit);
    });
    MyThread *thread = new MyThread;

    connect(this,&Widget::send_myAnswer,thread,&MyThread::set_intList_strList);
    connect(ui->pB_sub,&QPushButton::clicked,this,[=](){
            if (ui->lineEdit->text().isEmpty())
            {
                QMessageBox::information(this, "提示", "请填写API");
                return ;
            }
            if(ui->pB_sub->text()=="提交")
            {
                int max = ui->spinBox->value();    // 获取 当前抽取题目 数量
                ui->progressBar->setRange(0,max);   // 设置 进度条最大值为 获取的数量
                ui->pB_sub->setEnabled(true);

                qDebug()<<"m_answer_l"<<m_answer_l;
                emit send_myAnswer(m_selectlist,m_answer_l,ui->lineEdit->text());
                thread->start();
                ui->pB_sub->setText("提交评分");
            }
            else if(ui->pB_sub->text()=="提交评分")
            {
                qDebug()<<"m_grade_l"<<m_grade_l;
               // 向数据库 提交评分
               Database myDb("mydatabase.db","questions");
               QStringList columns;
               columns << "id" <<"次数"<<"总分"<<"平均";
               for(int i=0;i<m_selectlist.size();i++)
               {
                   QString queryCondition = "id = %1"; // 根据实际情况设置条件
                   int id = m_selectlist.at(i);
                   queryCondition = queryCondition.arg(id);

                   QList<QMap<QString, QVariant>> dataList = myDb.query_data(columns, queryCondition,0);
                   if (!dataList.isEmpty())
                   {
                       qDebug() << "Query result:";
                       for (const QMap<QString, QVariant> &data : dataList)
                       {
                           QMap<QString, QVariant> updateData;
                           updateData["总分"]=data["总分"].toInt()+(m_grade_l.at(i)-1)*30;
                           updateData["次数"]=data["次数"].toInt()+1;
                           updateData["平均"]=static_cast<float>(updateData["总分"].toInt()/updateData["次数"].toInt()) ;
                            qDebug() <<"id:[ "<<id<<" ] 总分 [ "<<updateData["总分"].toInt();
                            qDebug() <<"id:[ "<<id<<" ] 次数 [ "<<updateData["次数"].toInt();
                            qDebug() <<"id:[ "<<id<<" ] 平均 [ "<<updateData["平均"].toFloat();
                           if (myDb.update_one(updateData, queryCondition)) {
                               qDebug() <<"id:[ "<<id<<" ] m_grade_l Data updated successfully.";
                           } else {
                               qDebug() <<"id:[ "<<id<<" ] Failed to update data. m_grade_l";
                           }
                       }
                   }
                   else
                   {
                       qDebug() << "No data found."<<__LINE__<<__func__;
                   }
               }
                ui->pB_0->setEnabled(false);
                ui->pB_30->setEnabled(false);
                ui->pB_60->setEnabled(false);
                ui->pB_90->setEnabled(false);
               ui->pB_sub->setText("重置");
            }
            else
            {
                Widget::widgetInit();
            }

    });
    connect(thread,&MyThread::send_result_list,this,&Widget::get_reslut_l);
    connect(thread,&MyThread::send_waitTime,this,&Widget::get_waitTime);

    connect(ui->pB_0,&QPushButton::clicked,[=](){
           int now=ui->lb_num->text().toInt();
           while(m_grade_l.size()<now)
               m_grade_l.append(0);
           m_grade_l[now-1]=1;
           QPushButton *targetButton = buttonMap.value(QString::number(now));
           click_btn(targetButton);
        });
    connect(ui->pB_30,&QPushButton::clicked,[=](){
       int now=ui->lb_num->text().toInt();
       while(m_grade_l.size()<now)
           m_grade_l.append(0);
       m_grade_l[now-1]=2;
       QPushButton *targetButton = buttonMap.value(QString::number(now));
       click_btn(targetButton);
    });
    connect(ui->pB_60,&QPushButton::clicked,[=](){
       int now=ui->lb_num->text().toInt();
       while(m_grade_l.size()<now)
           m_grade_l.append(0);
       m_grade_l[now-1]=3;
       QPushButton *targetButton = buttonMap.value(QString::number(now));
       click_btn(targetButton);
    });
    connect(ui->pB_90,&QPushButton::clicked,[=](){
       int now=ui->lb_num->text().toInt();
       while(m_grade_l.size()<now)
           m_grade_l.append(0);
       m_grade_l[now-1]=4;
       QPushButton *targetButton = buttonMap.value(QString::number(now));
       click_btn(targetButton);
    });
    connect(ui->pB_save_API,&QPushButton::clicked,this,[=](){
        if(ui->pB_save_API->text()=="保存")
        {
             ui->lineEdit->setEnabled(false);
             ui->pB_save_API->setText("修改");
        }
        else
        {
            ui->lineEdit->setEnabled(true);
            ui->pB_save_API->setText("保存");
        }
    });
    connect(thread,&MyThread::send_progressBar,this,[=](int value){
       ui->progressBar->setValue(value);
    });
    connect(thread,&MyThread::send_part_result_list,this,[=](QStringList list){
        m_result_l.clear();
        foreach (const QString &str, list) {
            m_result_l.append(str);
        }
    });
}

Widget::~Widget()
{
    delete ui;
}
void Widget::widgetInit()
{
//    this->PythonInit();
    ui->progressBar_time->hide();
    ui->label_5->hide();

    ui->pB_last->setEnabled(false);
    ui->pB_next->setEnabled(false);
    ui->pB_sub->setEnabled(false);  // 设置 按钮逻辑
     int max_i = 0;     // 设置 最大抽取数量
     // 获取最大条目数
    Database myDb("mydatabase.db","questions");
    QStringList columns;
    columns << "*";
    QString queryCondition = "1=1";
    QList<QMap<QString, QVariant>> dataList = myDb.query_data(columns, queryCondition,0);
    if (!dataList.isEmpty())
    {
        qDebug() << "Query result:";
        for (const QMap<QString, QVariant> &data : dataList)
        {
            max_i+=1;
        }
    }
    else
    {
        qDebug() << "No data found."<<__LINE__<<__func__;
    }
     // 获取最大条目数 end

    ui->spinBox->setValue(0);
    ui->spinBox->setEnabled(true);

    ui->pB_start->setEnabled(true);
    click_btn(ui->pB_start);// 重置 抽取
    ui->pB_start->setEnabled(true);

    qDebug()<<"[spinBox max value 最大条目数]"<<max_i;
    ui->spinBox->setMaximum(max_i);
    ui->pB_sub->setText("提交");
    ui->pB_save_qstion->setEnabled(false);

    ui->lineEdit->setEnabled(true); // API编辑栏 允许编辑
    ui->textEdit->clear();
    ui->textBrowser->clear();

    ui->pB_0->setStyleSheet("");
    ui->pB_30->setStyleSheet("");
    ui->pB_60->setStyleSheet("");
    ui->pB_90->setStyleSheet("");

    ui->pB_0->setEnabled(false);
    ui->pB_30->setEnabled(false);
    ui->pB_60->setEnabled(false);
    ui->pB_90->setEnabled(false);

    ui->progressBar->setValue(0);

    m_selectlist.clear();
    m_grade_l.clear();
    m_qstion_l.clear();
    m_answer_l.clear();
}
// model 1 从数据库按比例抽题（表 id 值） 并 存储于私有属性 m_selectlist 中
void Widget::select_items(int select_num)
{
    Database myDb("mydatabase.db","questions");
    myDb.change_table("qstion_info");
    QStringList columns;
    columns << "id" <<"数量"<<"位置";
    QString queryCondition = "1 = 1"; // 根据实际情况设置条件
     //  1. 获取 辅表中每一项的 id 和 数量
    QList<QMap<QString, QVariant>> dataList = myDb.query_data(columns, queryCondition,0);

    if (!dataList.isEmpty())
    {
        qDebug() <<"- qstion_info open success[ 辅表信息获取正常 ]";
        int table_max_num=0;    // 统计辅表中记录的 “数量” 总和
        for (const QMap<QString, QVariant> &data : dataList)
        {
            int table_num = data["数量"].toInt();
            table_max_num+=table_num;
            qDebug() << "-- [ table_num 数量 ] :" << table_num<< "[ table_max_num 总量 ] :" << table_max_num;
        }
        float percent =  static_cast<float>(select_num)/table_max_num;
        qDebug() << "- [ percent 占比百分比为 ] : "<< percent;

        QList<QMap<QString, QVariant>> showListMap;
        if(percent>=1)
        {
            // 有多少出多少
            for ( const QMap<QString,QVariant> data:dataList ) {
                QMap<QString,QVariant> item;
                item["id"]=data["id"];
                int new_num = data["数量"].toInt(); // 向下取整
                item["数量"]=new_num;
                qDebug() << "--- [ id ] : "<<  item["id"]<<"[ item['数量'] ]"<<new_num;
                showListMap.append(item);
            }
        }
        else
        {
            int fix_num=select_num;
            for ( const QMap<QString,QVariant> data:dataList ) {
                QMap<QString,QVariant> item;
                item["id"]=data["id"];
                int new_num = static_cast<int>(floor(data["数量"].toInt()*percent)); // 向下取整
                item["数量"]=new_num;
                qDebug() << "--- [ id ] : "<<  item["id"]<<"[ item['数量'] ]"<<new_num;
                showListMap.append(item);
                fix_num-=new_num;
            }
            qDebug() << "-- [fix_num 修正值] : "<< fix_num;
            for( int i=0 ; i<fix_num ; i++ )
            {
                showListMap[i]["数量"]=showListMap[i]["数量"].toInt()+1;
            }

            for ( const QMap<QString,QVariant> data:showListMap ) {
                qDebug() << "--- [ id ] : "<<  data["id"]<<"[ item['数量'] ]"<<data["数量"];
            }

        }

        // 根据 showlistmap 中的数量 从 datalist中 的位置字段 选出 n个数据
        int index=0;
        for (const QMap<QString, QVariant> &data : dataList)
        {
            QStringList list;
            list = data["位置"].toString().split(",");
            int n = list.size();
            // 将 内容乱序排列
            for (int i = n - 1; i > 0; --i)
            {
                int j = QRandomGenerator::global()->bounded(i + 1);
                list.swapItemsAt(i, j);
            }
            // 将 n 复用为 最大取出数
            n = showListMap[index]["数量"].toInt();
            for (const QString &str : list)
            {
                if (n<1)break;
                int intValue = str.toInt();
                m_selectlist.append(intValue);
                n--;
             }
            index++;
        }
        qDebug()<<"-- [ finaly select list 最终选择数组为 ]"<<m_selectlist;
    }
    else
    {
        qDebug() << "No data found."<<__LINE__<<__func__;
    }

}
void Widget::click_btn(QPushButton* btn)
{
    QMouseEvent *clickEvent = new QMouseEvent(QEvent::MouseButtonPress,btn->rect().center(),
                                                  Qt::LeftButton,
                                                  Qt::LeftButton,
                                                  Qt::NoModifier);
    QApplication::sendEvent(btn, clickEvent);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease,
                                                btn->rect().center(),
                                                Qt::LeftButton,
                                                Qt::LeftButton,
                                                Qt::NoModifier);
    QApplication::sendEvent(btn, releaseEvent);
    delete clickEvent;
    delete releaseEvent;
}
void Widget::showMessageDialog(const QString& title, const QString& text) {
    QMessageBox messageBox;
    messageBox.setWindowTitle(title);
    messageBox.setText(text);
    messageBox.exec();
}

void Widget::get_reslut_l(QStringList  reslt_l)
{
    m_result_l.clear();
    foreach (const QString &str, reslt_l) {
        m_result_l.append(str);
    }
    showMessageDialog("提示","答案获取完毕，请点击题号按钮查看吧");
    ui->pB_0->setEnabled(true);
    ui->pB_30->setEnabled(true);
    ui->pB_60->setEnabled(true);
    ui->pB_90->setEnabled(true);

    qDebug()<<"m_result"<<m_result_l;
}
void Widget::get_waitTime(int time)
{
    // 创建一个QTimer对象
    qDebug()<<"time"<<time;
    QTimer* timer = new QTimer(this);
    ui->progressBar_time->show();
    ui->label_5->show();
    ui->progressBar_time->setRange(0,time*10);
    ui->progressBar_time->setValue(0);
    timer->setInterval(100);
    connect(this,&Widget::progress_hide,this,[=](){
        timer->stop();
        ui->progressBar_time->hide();
        ui->label_5->hide();
    });
    // 连接超时信号和槽
    QObject::connect(timer, &QTimer::timeout, [&](){
        int value=ui->progressBar_time->value();
        qDebug()<<"value"<<value<<"maximum"<<ui->progressBar_time->maximum();
        ui->progressBar_time->setValue(value+1);
        if(value == ui->progressBar_time->maximum() )
        {
            emit progress_hide();
        }
    });
    timer->start();
}
void Widget::test(){


}
