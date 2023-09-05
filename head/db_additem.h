#ifndef DB_ADDITEM_H
#define DB_ADDITEM_H

#include <QWidget>

namespace Ui {
class DB_AddItem;
}

class DB_AddItem : public QWidget
{
    Q_OBJECT

public:
    explicit DB_AddItem(QWidget *parent = nullptr);
    static int success;
    static int fail;
    ~DB_AddItem();
    void init();
    void showMessageDialog(const QString& title, const QString& text);
    void status(int s,int f);
signals:
    void refalshWidgtInit();
private:
    Ui::DB_AddItem *ui;
};

#endif // DB_ADDITEM_H
