#ifndef TRANSFORMSETTINGSDIALOG_H
#define TRANSFORMSETTINGSDIALOG_H

#include <QDialog>
#include <QButtonGroup>

#include "TransformSettings.h"

namespace Ui {
class TransformSettingsDialog;
}

class TransformSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransformSettingsDialog(QWidget *parent = 0);
    ~TransformSettingsDialog();

    int GetCropLeft() const;
    int GetCropTop() const;
    int GetCropRight() const;
    int GetCropBottom() const;

    bool IsResizeActivated();
    int GetResizeWidth();
    int GetResizeHeight();

    eTransformRotateType GetRotateType() const;

    void setTransformSettings(const TransformSettings& transformSettings);

signals:
    void settingsApplied();

protected slots:
    void onResizeChecked(int state);

    void onApplyPressed();
    void onCancelPressed();

private:
    Ui::TransformSettingsDialog *ui;

    QButtonGroup* _rotateGroup = nullptr;
};

#endif // TRANSFORMSETTINGSDIALOG_H
