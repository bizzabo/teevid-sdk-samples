#ifndef FRIENDITEM_H
#define FRIENDITEM_H

#include <QWidget>

namespace Ui {
class FriendItem;
}

class FriendItem : public QWidget
{
    Q_OBJECT

public:
    explicit FriendItem(QWidget *parent = 0);
    ~FriendItem();

    void setName(QString name);
    QString getName() const;

    void setPhone(QString phone);
    QString getPhone() const;

private:
    Ui::FriendItem *ui;
};

#endif // FRIENDITEM_H
