#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Contact.h"
#include <vector>

class VideoManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    std::vector<Contact> getContacts();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
