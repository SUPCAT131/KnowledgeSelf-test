#include "../head/db_additem.h"
#include "../head/database.h"
#include "../head/widget.h"
#include "ui_db_additem.h"
#include <QtDebug>
#include <QMessageBox>

//int DB_AddItem::success = 0;
//int DB_AddItem::fail = 0;

DB_AddItem::DB_AddItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DB_AddItem)
{
    set_sf();
    ui->setupUi(this);
    DB_AddItem::init();
    // 小类的选项随大类变化而变化
    connect(ui->CBoxBig,&QComboBox::currentTextChanged,[=](){
        Database db("mydatabase.db","questions");

        // 获取小类列表 根据大类的名称
        QList<QString> subCategory;
        QString name = ui->CBoxBig->currentText();
        QStringList columns;
        columns <<"小类";
        QString queryCondition = "分类 = '%1'";
        queryCondition=queryCondition.arg(name);
        QList<QMap<QString, QVariant>> dataList = db.query_data(columns, queryCondition,1);
        if (!dataList.isEmpty()) {
            qDebug() << "Query result:";
        for (const QMap<QString, QVariant> &data : dataList)
        {
           subCategory.append(data["小类"].toString());
        }
        } else {
            qDebug() << "No data found."<<__LINE__<<__func__;
        }

        ui->CBoxSml->clear();
        ui->CBoxSml->addItems(subCategory);

    });

    // 刷新页面
    connect(ui->ButtonRe,&QPushButton::clicked,[=](){
       DB_AddItem::init();
    });

    // 上传数据
    connect(ui->ButtonSub, &QPushButton::clicked, [=]() {
        ui->labelStatus->setText("上传处理中");
        Database myDb("mydatabase.db","questions");

        QString qustion = ui->lineEditPrstion->text();
        QString fathers = ui->lineEditBig->text();
        QString children = ui->lineEditSml->text();
        if (fathers.isEmpty())
            fathers = ui->CBoxBig->currentText();
        if (children.isEmpty())
            children = ui->CBoxSml->currentText();
        if (qustion.isEmpty())
        {
            this->showMessageDialog("提示","请填写题目");
            this->status(0,1);
        }
        else if (fathers.isEmpty())
        {
            this->showMessageDialog("提示", "请选择或输入主类项");
            this->status(0,1);
        }
        else if (children.isEmpty())
        {
            this->showMessageDialog("提示", "请选择或输入子类项");
            this->status(0,1);
        }
        else
        {
            QMap<QString, QVariant> insertData;
            insertData["分类"] = fathers;
            insertData["小类"] = children;
            insertData["题目"] = qustion;
            insertData["参考"] =  ui->textEditanser->toPlainText();

            if (myDb.add_one(insertData)) {
                qDebug() << "Data inserted successfully.";
                myDb.info_add(insertData);
                this->showMessageDialog("提示", "添加成功");
                this->status(1,0);
                emit refalshWidgtInit();
            } else {
                qDebug() << "Failed to insert data.";
                this->showMessageDialog("提示", "添加失败");
                this->status(0,1);
            }
        }

    });

}

DB_AddItem::~DB_AddItem()
{
    delete ui;
}

void DB_AddItem::init()
{
    Database myDb("mydatabase.db","questions");
    QList<QString> category;
    QStringList columns;
    columns <<"分类";
    QString queryCondition = "1 = 1";
    QList<QMap<QString, QVariant>> dataList = myDb.query_data(columns, queryCondition,1);
    if (!dataList.isEmpty()) {
        qDebug() << "Query result:";
    for (const QMap<QString, QVariant> &data : dataList)
    {
        category.append(data["分类"].toString());
    }
    } else {
        qDebug() << "No data found."<<__LINE__<<__func__;
    }

    ui->CBoxBig->clear();
    ui->CBoxSml->clear();
    ui->CBoxBig->addItems(category);

    ui->lineEditBig->clear();
    ui->lineEditSml->clear();
    ui->textEditanser->clear();
}
void DB_AddItem::showMessageDialog(const QString& title, const QString& text) {
    QMessageBox messageBox;
    messageBox.setWindowTitle(title);
    messageBox.setText(text);
    messageBox.exec();
}
void DB_AddItem::status(int s=0,int f=0)
{
    m_success+=s;
    m_false+=f;
    QString lb_status = "成功:" + QString::number(m_success) + "条/失败:" + QString::number(m_false) + "条";
    ui->labelStatus->setText(lb_status);
}

void DB_AddItem::set_sf()
{
    m_success=0;
    m_false=0;
}
