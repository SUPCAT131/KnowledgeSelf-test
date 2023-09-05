#include "../head/db_additem.h"
#include "../head/database.h"
#include "../head/widget.h"
#include "ui_db_additem.h"
#include <QtDebug>
#include <QMessageBox>

int DB_AddItem::success = 0;
int DB_AddItem::fail = 0;

DB_AddItem::DB_AddItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DB_AddItem)
{
    ui->setupUi(this);
    DB_AddItem::init();
    // 小类的选项随大类变化而变化
    connect(ui->CBoxBig,&QComboBox::currentTextChanged,[=](){
        database myDb;
        QString path = "mydatabase.db";
        QSqlDatabase db = myDb.opendb(path);

        QList<QString> subCategory;
        QString name = ui->CBoxBig->currentText();
//        QString ordname = "小类";
        subCategory = myDb.DBSearch(db,name,"分类","小类");
        ui->CBoxSml->clear();
        ui->CBoxSml->addItems(subCategory);
        db.close();
    });

    // 刷新页面
    connect(ui->ButtonRe,&QPushButton::clicked,[=](){
       DB_AddItem::init();
    });

    // 上传数据
    connect(ui->ButtonSub, &QPushButton::clicked, [=]() {
        ui->labelStatus->setText("上传处理中");
        database myDb;
        QString path = "mydatabase.db";
        QSqlDatabase db = myDb.opendb(path);

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
            bool ck = myDb.add_db(db, fathers, children, qustion, ui->textEditanser->toPlainText());
            if (ck)
            {
                this->showMessageDialog("提示", "添加成功");
                this->status(1,0);
                emit refalshWidgtInit();
            }
            else
            {
                this->showMessageDialog("提示", "添加失败");
                this->status(0,1);
            }
        }
        db.close();
    });

}

DB_AddItem::~DB_AddItem()
{
    delete ui;
}

void DB_AddItem::init()
{
    database myDb;
    QString path = "mydatabase.db";
    QSqlDatabase db = myDb.opendb(path);

    QList<QString> category;
    category = myDb.DBSearch(db);
    ui->CBoxBig->clear();
    ui->CBoxSml->clear();
    ui->CBoxBig->addItems(category);

    db.close();
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
    this->success+=s;
    this->fail+=f;
    QString lb_status = "成功:" + QString::number(success) + "条/失败:" + QString::number(fail) + "条";
    ui->labelStatus->setText(lb_status);
}
