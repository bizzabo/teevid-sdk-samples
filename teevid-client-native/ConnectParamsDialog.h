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
    QString GetToken() const;
    QString GetRoom() const;
    QString GetUser() const;
    QString GetPassword() const;
    int GetAccessPin() const;

signals:
    void paramsApplied();
    void paramsCancelled();

protected slots:
    void OnBtnApply();
    void OnBtnCancel();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::ConnectParamsDialog *ui;
    bool _paramsApplied = false;
};

#endif // CONNECTPARAMSDIALOG_H
