#ifndef FIXADD_H
#define FIXADD_H

#include <QWidget>
#include <QEventLoop>
namespace Ui {
class FixAdd;
}

class FixAdd : public QWidget
{
    Q_OBJECT

public:
    explicit FixAdd(QWidget *parent = nullptr);
    ~FixAdd();
    int exec();
signals:
    void send_msg(QString);
public slots:
    void set_msg(QString msg);
    void onSubmitClicked();
private:
    Ui::FixAdd *ui;
    QString m_msg;
   QEventLoop *m_eventLoop;
};

#endif // FIXADD_H
