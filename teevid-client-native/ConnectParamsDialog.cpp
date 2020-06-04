#include "ConnectParamsDialog.h"
#include "ui_ConnectParamsDialog.h"

#include <QMessageBox>
#include <QSettings>

ConnectParamsDialog::ConnectParamsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectParamsDialog)
{
    ui->setupUi(this);

    setModal(true);
    setWindowTitle("Connection parameters");

    QSettings settings("teevid-client-native", "connect_params");
    settings.beginGroup("teevid-client-native");
    ui->lineEditHost->setText(settings.value("host", "").toString());
    ui->lineEditRoom->setText(settings.value("room", "").toString());
    ui->lineEditUser->setText(settings.value("user", "").toString());
    settings.endGroup();

    connect(ui->btnApply, SIGNAL(pressed()), this, SLOT(OnBtnApply()));
    connect(ui->btnCancel, SIGNAL(pressed()), this, SLOT(OnBtnCancel()));
}

ConnectParamsDialog::~ConnectParamsDialog()
{
    delete ui;
}

QString ConnectParamsDialog::GetHost() const
{
    return ui->lineEditHost->text();
}

QString ConnectParamsDialog::GetRoom() const
{
    return ui->lineEditRoom->text();
}

QString ConnectParamsDialog::GetUser() const
{
    return ui->lineEditUser->text();
}

void ConnectParamsDialog::OnBtnApply()
{
    QString host = ui->lineEditHost->text();
    if (host.isEmpty())
    {
        QMessageBox mb(QMessageBox::Critical, "Error", "Please enter server host");
        mb.exec();
        return;
    }

    QString room = ui->lineEditRoom->text();
    if (room.isEmpty())
    {
        QMessageBox mb(QMessageBox::Critical, "Error", "Please enter room name");
        mb.exec();
        return;
    }

    QString user = ui->lineEditUser->text();
    if (user.isEmpty())
    {
        QMessageBox mb(QMessageBox::Critical, "Error", "Please enter your name");
        mb.exec();
        return;
    }

    QSettings settings("teevid-client-native", "connect_params");
    settings.beginGroup("teevid-client-native");
    settings.setValue("host", host);
    settings.setValue("room", room);
    settings.setValue("user", user);
    settings.endGroup();

    _paramsApplied = true;
    close();
}

void ConnectParamsDialog::OnBtnCancel()
{
    exit(0);
}

void ConnectParamsDialog::closeEvent(QCloseEvent *event)
{
    emit _paramsApplied ? paramsApplied() : paramsCancelled();
    QDialog::closeEvent(event);
}
