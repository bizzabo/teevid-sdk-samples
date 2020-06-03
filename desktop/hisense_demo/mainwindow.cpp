#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "InitialScreen.h"

#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Hisense Demo");

    InitialScreen *initialScreen = new InitialScreen(this);
    std::vector<Contact> contacts = getContacts();
    initialScreen->setFriendsData(contacts);

    ui->verticalLayout->addWidget(initialScreen);
    ui->verticalLayout->setStretchFactor(initialScreen, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<Contact> MainWindow::getContacts()
{
    std::vector<Contact> contacts;

    Contact c1;
    c1.name = "Aaria Arredondo";
    c1.phone = "185 2544 8962";
    contacts.push_back(c1);

    Contact c2;
    c2.name = "Addison Stewart";
    c2.phone = "135 3685 2447";
    contacts.push_back(c2);

    Contact c3;
    c3.name = "Barbara Anlisadria";
    c3.phone = "156 7852 4122";
    contacts.push_back(c3);

    return contacts;
}
