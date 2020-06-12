#include "FriendItem.h"
#include "ui_FriendItem.h"

FriendItem::FriendItem(QWidget *parent) : QWidget(parent), ui(new Ui::FriendItem)
{
    ui->setupUi(this);

    QString styleSheet = "QLabel#labelName { color: #000000; font-size: 13px; }"
                         "QLabel#labelPhone { color: #cccccc; font-size: 11px; }";
    setStyleSheet(styleSheet);
}

FriendItem::~FriendItem()
{
    delete ui;
}


void FriendItem::setName(QString name)
{
    ui->labelName->setText(name);
}


QString FriendItem::getName() const
{
    return ui->labelName->text();
}


void FriendItem::setPhone(QString phone)
{
    ui->labelPhone->setText(phone);
}


QString FriendItem::getPhone() const
{
    return ui->labelPhone->text();
}
