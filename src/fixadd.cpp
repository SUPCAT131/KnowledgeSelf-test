#include "../head/fixadd.h"
#include "ui_fixadd.h"

#include <QtDebug>
FixAdd::FixAdd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FixAdd)
{
    ui->setupUi(this);

    qDebug()<<"1new window msg"<<m_msg;
    connect(ui->pb_sub,&QPushButton::clicked,this,&FixAdd::onSubmitClicked);


}

FixAdd::~FixAdd()
{
    delete ui;
}

void FixAdd::set_msg(QString msg)
{
    m_msg = msg;
    qDebug()<<"2new window msg add2"<<m_msg;
}

int FixAdd::exec()
{
    // Create a new event loop
    QEventLoop eventLoop;
    m_eventLoop = &eventLoop;
    qDebug()<<"3new window msg on show"<<m_msg;
    // Show the dialog
    show();
    ui->textEdit_fixadd->setText(m_msg);
    // Enter the event loop and wait until the dialog is closed
    int result = eventLoop.exec();
    m_eventLoop = nullptr;

    return result;
}

void FixAdd::onSubmitClicked()
{
    if (m_eventLoop) {
        // Emit the send_msg signal
        emit send_msg(ui->textEdit_fixadd->toPlainText());

        // Close the dialog and exit the event loop
        close();
        m_eventLoop->exit();
    }
}
