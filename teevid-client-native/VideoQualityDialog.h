#ifndef VIDEOQUALITYDIALOG_H
#define VIDEOQUALITYDIALOG_H

#include <QDialog>

namespace Ui {
class VideoQualityDialog;
}

class QButtonGroup;

class VideoQualityDialog : public QDialog
{
    Q_OBJECT

    enum eQuality
    {
        Low = 0,
        High = 1
    };

public:
    explicit VideoQualityDialog(QWidget *parent = 0);
    ~VideoQualityDialog();

signals:
    void lowQualitySelected();
    void highQualitySelected();

protected slots:
    void onQualitySelected(int optionId);

private:
    Ui::VideoQualityDialog *ui;

    QButtonGroup* _radioGroup = nullptr;
};

#endif // VIDEOQUALITYDIALOG_H
