#ifndef CONNECTPARAMSDIALOG_H
#define CONNECTPARAMSDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class ConnectParamsDialog;
}

class ConnectParamsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectParamsDialog(QWidget *parent = nullptr);
    ~ConnectParamsDialog();

    QString GetHost() const;
    QString GetRoom() const;
    QString GetUser() const;

signals:
    void paramsApplied();

protected slots:
    void OnBtnApply();
    void OnBtnCancel();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::ConnectParamsDialog *ui;
};

#endif // CONNECTPARAMSDIALOG_H
