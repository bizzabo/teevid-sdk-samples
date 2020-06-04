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
    std::string roomName = ui->lineEditRoomId->text().toUtf8().constData();
    if (roomName.empty())
    {
        QMessageBox mb(QMessageBox::Critical, "Error", "Please enter a valid room ID");
        mb.exec();
        return;
    }
    emit roomNameSubmitted(roomName);
    close();
}
