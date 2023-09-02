#include "widget.h"
#include "ui_widget.h"
#include <QtDebug>
#include <QScrollArea>
#include "database.h"
#include <QSqlQuery>
#include <QThread>
#include <QMouseEvent>
#include <QMessageBox>
#include "db_additem.h"
#include <QSqlError>

static QList<int> selectlist;
static QList<int> grade_l;
static QList<QString> qstion_l;
static QList<QString> answer_l;
static QString db_path;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    widgetInit();
    connect(ui->pB_sub, &QPushButton::clicked, this, &Widget::onStartClicked);
    connect(ui->pB_start, &QPushButton::clicked, [=]() {
        int numberOfButtons = ui->spinBox->value(); // 获取用户输入的按钮数量
        database myDb;
        QSqlDatabase db = myDb.opendb(db_path);
        selectlist=myDb.DBSelect(db,numberOfButtons,"无","无");

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

        for (int i = 0; i < numberOfButtons; ++i) {
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
                qDebug() << "Button " << (i + 1) << " clicked!";
                int num;
                ui->pB_save_qstion->setEnabled(true);
                if(i+1>selectlist.size())
                    num = 0;
                else
                    num = selectlist.at(i);
                qDebug() << num;
                database myDb;
                QSqlDatabase db = myDb.opendb(db_path);
                this->setqstion(db,num);
                ui->textEdit->clear();
                ui->textBrowser->clear();
                ui->lb_num->setNum(i+1);
                if(qstion_l.size()>i)
                {
                    ui->textEdit->setText(qstion_l[i]);
                    ui->textBrowser->setText(answer_l[i]);
                }
                ui->pB_0->setStyleSheet("");
                ui->pB_30->setStyleSheet("");
                ui->pB_60->setStyleSheet("");
                ui->pB_90->setStyleSheet("");
                QString style = "border-color: rgb(46, 204, 113);border-width:2px;border-style: outset;background-color: rgb(0, 150, 0);";
                if(grade_l.size()>i)
                    switch (grade_l.at(i))
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
                if(i==numberOfButtons-1)
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
    });
    connect(ui->pB_save_qstion,&QPushButton::clicked,[=](){
        int now = ui->lb_num->text().toInt();
        int now_q = qstion_l.size();
        int now_a = answer_l.size();
        QString anser = ui->textEdit->toPlainText();
        while(now_q<now)
        {
            qstion_l.append("");
            now_q = qstion_l.size();
            qDebug()<<"now="<<now<<"now_q"<<now_q<<"qstion_l: "<<qstion_l;
        }
        while (now_a<now)
        {
            answer_l.append("");
            now_a = answer_l.size();
            qDebug()<<"now="<<now<<"now_a"<<now_a<<"answer_l: "<<answer_l;
        }
        qstion_l[now-1]=anser;

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
        click_num_btn(now);
    });
    connect(ui->pB_next,&QPushButton::clicked,[=](){
        int now = ui->lb_num->text().toInt()+1;
        click_num_btn(now);
    });
    connect(ui->pB_0,&QPushButton::clicked,[=](){
       int now=ui->lb_num->text().toInt();
       while(grade_l.size()<now)
           grade_l.append(0);
       grade_l[now-1]=1;
       click_num_btn(now);
    });
    connect(ui->pB_30,&QPushButton::clicked,[=](){
       int now=ui->lb_num->text().toInt();
       while(grade_l.size()<now)
           grade_l.append(0);
       grade_l[now-1]=2;
       click_num_btn(now);
    });
    connect(ui->pB_60,&QPushButton::clicked,[=](){
       int now=ui->lb_num->text().toInt();
       while(grade_l.size()<now)
           grade_l.append(0);
       grade_l[now-1]=3;
       click_num_btn(now);
    });
    connect(ui->pB_90,&QPushButton::clicked,[=](){
       int now=ui->lb_num->text().toInt();
       while(grade_l.size()<now)
           grade_l.append(0);
       grade_l[now-1]=4;
       click_num_btn(now);
    });
    connect(ui->pB_addItem,&QPushButton::clicked,[=](){
        DB_AddItem *db_AddItem = new DB_AddItem(nullptr);
        db_AddItem->show();
        showMessageDialog("提示","注意 添加成功后会[重置]答题界面");
        connect(db_AddItem,&DB_AddItem::refalshWidgtInit,this,&Widget::widgetInit);
    });
    connect(ui->pB_save_API,&QPushButton::clicked,[=](){
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


}

Widget::~Widget()
{
    delete ui;
}
void Widget::widgetInit()
{
    this->PythonInit();
    db_path = "mydatabase.db";
    ui->pB_last->setEnabled(false);
    ui->pB_next->setEnabled(false);
    ui->pB_sub->setEnabled(false);  // 设置 按钮逻辑
    database myDb;
    QSqlDatabase db = myDb.opendb(db_path);
    int max_i = getmaxItem(db);     // 设置 最大抽取数量
    ui->spinBox->setValue(0);

    ui->pB_start->setEnabled(true);
    Widget::click_btn(ui->pB_start);// 重置 抽取
    ui->pB_start->setEnabled(true);

    qDebug()<<max_i;
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

    ui->progressBar->setValue(0);

    selectlist.clear();
    grade_l.clear();
    qstion_l.clear();
    answer_l.clear();
}
void Widget::setqstion(QSqlDatabase db, int num)
{
    if(num==0)
    {
        ui->lb_qstion->setText("xxx问题");
        ui->lb_childname->setText("小类");
        ui->lb_fthrname->setText("分类");
        return;
    }
    if (!db.open()) {
           qDebug() << "无法连接到数据库。"<<__func__<<__LINE__;
           return;
       }

       QSqlQuery query(QString("SELECT 分类, 小类, 题目 FROM questions WHERE id = %1").arg(num));
       if (query.next())
       {
           QString category = query.value(0).toString();
           QString subcategory = query.value(1).toString();
           QString question = query.value(2).toString();

           qDebug() << "分类：" << category;
           qDebug() << "小类：" << subcategory;
           qDebug() << "问题：" << question;
           ui->lb_qstion->setText(question);
           ui->lb_childname->setText(subcategory);
           ui->lb_fthrname->setText(category);
       }
       else
       {
           qDebug() << "找不到 id=" << num << " 的数据。";
       }

       db.close();
}
void Widget::onStartClicked()
{
    if(ui->lineEdit->text().isEmpty())
    {
        showMessageDialog("提示","请填写API");
        return;
    }
    if(ui->pB_sub->text()=="提交")
    {
        int max = ui->spinBox->value();
        ui->progressBar->setRange(0,max);
        for (int i=0;i<max;i++) {
            workQueue(i);
            QThread::sleep(20);
            ui->progressBar->setValue(i+1);
        }
        ui->pB_sub->setText("提交评分");
    }
    else if(ui->pB_sub->text()=="提交评分")
    {
       subGrad();
       ui->pB_sub->setText("重置");
    }
    else
    {
        Widget::widgetInit();
    }
}
void Widget::workQueue(int i)
{
    // 创建新线程
    QThread* thread = new QThread;

    // 创建自定义的 Worker 对象，用于在新线程中执行操作
    Worker* worker = new Worker;
    worker->moveToThread(thread);

    // 连接操作完成信号和关闭线程的槽函数
    connect(worker, &Worker::workFinished, thread, &QThread::quit);
    connect(worker, &Worker::workFinished,[=](){
//        int now = ui->lb_num->text().toInt();
        int now_a = answer_l.size();
        if(i+1==now_a)
            ui->textBrowser->setText(answer_l[i]);
    });
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    // 启动线程，并传递需要的参数
    thread->start();

    database myDb;
    QSqlDatabase db = myDb.opendb(db_path);
    int max_i = getmaxItem(db);
    if(max_i==0||selectlist.at(i)>max_i) return;
    QString qstion = getqustion(db,selectlist.at(i));
    QString anser = qstion_l.at(i);
    // 发送开始执行操作的信号
    QString api = ui->lineEdit->text();
    emit worker->startWork(qstion,anser,i,api);
}
QString Widget::getqustion(QSqlDatabase db,int num)
{
    if (!db.open()) {
           qDebug() << "无法连接到数据库。"<<__func__<<__LINE__;
           return QString("n");
       }

       QSqlQuery query(QString("SELECT 分类, 小类, 题目 FROM questions WHERE id = %1").arg(num));
       if (query.next())
       {
           QString category = query.value(0).toString();
           QString subcategory = query.value(1).toString();
           QString question = query.value(2).toString();

           qDebug() << "分类：" << category;
           qDebug() << "小类：" << subcategory;
           qDebug() << "问题：" << question;
           db.close();
           return question;
       }
       else
       {
           qDebug() << "找不到 id=" << num << " 的数据。";
       }
       db.close();
       return QString("n");
}
void Worker::startWork(QString qstion,QString anser,int now,QString api)
{
    Worker work;
    // 执行操作，可以在这里进行数据库查询等耗时操作
    int now_q = qstion_l.size();
    int now_a = answer_l.size();
    while(now_q<now+1)
    {
        qstion_l.append("");
        now_q = qstion_l.size();
        qDebug()<<"now="<<now<<"now_q"<<now_q<<"qstion_l: "<<qstion_l;
    }
    while (now_a<now+1)
    {
        answer_l.append("");
        now_a = answer_l.size();
        qDebug()<<"now="<<now<<"now_a"<<now_a<<"answer_l: "<<answer_l;
    }
    qstion_l[now]=anser;
    QString gettext = send(qstion,anser,api);// 传递索引值 int now；
    answer_l[now]=gettext;
    // 操作完成，发出信号
    emit work.workFinished();
};
void Widget::click_num_btn(int num)
{
    QPushButton *targetButton = buttonMap.value(QString::number(num));
    QMouseEvent *clickEvent = new QMouseEvent(QEvent::MouseButtonPress,targetButton->rect().center(),
                                                  Qt::LeftButton,
                                                  Qt::LeftButton,
                                                  Qt::NoModifier);
    QApplication::sendEvent(targetButton, clickEvent);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease,
                                                targetButton->rect().center(),
                                                Qt::LeftButton,
                                                Qt::LeftButton,
                                                Qt::NoModifier);
    QApplication::sendEvent(targetButton, releaseEvent);
    delete clickEvent;
    delete releaseEvent;
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
int Widget::getmaxItem(QSqlDatabase db)
{
    int lastId = 0;
    if (!db.open()) {
        qDebug() << "无法打开数据库";
        return lastId;
    }

    QSqlQuery query;
    query.prepare("SELECT id FROM questions ORDER BY id DESC LIMIT 1");
    if (query.exec() && query.next()) {
        lastId = query.value(0).toInt();
        qDebug() << "最后一条记录的ID：" << lastId;
    } else {
        qDebug() << "获取最后一条记录的ID失败";
    }

    db.close();
    return lastId;
}

void Widget::onShowChild()
{
    db_AddItem->show();
}
void Widget::subGrad()
{
    int all=0;
    int max = ui->spinBox->value();

    for (int i =0;i<max;i++)
    {
        all+=(grade_l[i]-1)*30;
        int grade = Widget::db_get_num_byId(selectlist[i],"总分数");
        grade+=(grade_l[i]-1)*30;
        Widget::db_sub_grade(selectlist[i],grade);
    }
    all = all/max;
    QString msg = "本次测试总分为[ "+QString::number(all)+" ]分";
    this->showMessageDialog("提示",msg);

}
int Widget::db_get_num_byId(int num,QString item)
{
    database myDb;
    QSqlDatabase db = myDb.opendb(db_path);
    if (!db.open())
    {
       qDebug() << "无法连接到数据库。"<<__func__<<__LINE__;
       return -1;
    }
    int grade;
    QSqlQuery query(QString("SELECT %2 FROM questions WHERE id = %1").arg(num).arg(item));
    if (query.next())
    {
        grade = query.value(0).toInt();
        qDebug() << "分数：" << grade;
    }
    else
    {
       qDebug() << "找不到 id=" << num << " 的数据。";
       grade = -1;
    }
    db.close();
    return grade;
}
void Widget::db_sub_grade(int num,int grade)
{
    database myDb;
    QSqlDatabase db = myDb.opendb(db_path);
    if (!db.open())
    {
       qDebug() << "无法连接到数据库。"<<__func__<<__LINE__;
       return ;
    }
    QSqlQuery query(QString("UPDATE questions SET 总分数 = %2 WHERE id = %1").arg(num).arg(grade));
    if (query.exec())
    {
        qDebug() << "id ="<<num<< "[分数]更新成功。";
        int times = Widget::db_get_num_byId(num,"出题次数");
        times+=1;
        float abv = grade/times;
        database myDb;
        QSqlDatabase db = myDb.opendb(db_path);
        QSqlQuery query(QString("UPDATE questions SET 出题次数 = %2,平均分 = %3 WHERE id = %1").arg(num).arg(times).arg(abv));
        if (query.exec()) {
            qDebug() << "id ="<<num<< "[出题次数,平均分]更新成功。";
        } else {
            qDebug() << "更新失败：" << query.lastError().text();
        }
    }
    else
    {
        qDebug() << "更新失败：" << query.lastError().text();
    }
    db.close();
}
