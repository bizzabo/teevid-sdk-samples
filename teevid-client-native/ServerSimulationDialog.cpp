#include "ServerSimulationDialog.h"
#include "ui_ServerSimulationDialog.h"

#include <QMessageBox>

ServerSimulationDialog::ServerSimulationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerSimulationDialog)
{
    ui->setupUi(this);
    setWindowTitle("Server Simulation");

    connect(ui->btnIncomingCall, SIGNAL(pressed()), this, SLOT(onIncomingCallPressed()));
}

ServerSimulationDialog::~ServerSimulationDialog()
{
    delete ui;
}

void ServerSimulationDialog::onIncomingCallPressed()
{
    QString caller = ui->lineEditCaller->text();
    if (caller.isEmpty())
    {
        QMessageBox mb(QMessageBox::Critical, "Error", "Please enter caller name");
        mb.exec();
        return;
    }

    QString invitationUrl = ui->textEditToken->toPlainText();
    if (invitationUrl.isEmpty())
    {
        QMessageBox mb(QMessageBox::Critical, "Error", "Please enter your invitation URL");
        mb.exec();
        return;
    }
    emit roomSubmitted(caller, invitationUrl);
    close();
}
