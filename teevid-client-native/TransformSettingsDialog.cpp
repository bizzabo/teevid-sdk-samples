#include "TransformSettingsDialog.h"
#include "ui_TransformSettingsDialog.h"

#include "TransformSettings.h"

TransformSettingsDialog::TransformSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransformSettingsDialog)
{
    ui->setupUi(this);

    setWindowTitle("Transformation settings");

    ui->editCropLeft->setValidator(new QIntValidator(0, 9999, this));
    ui->editCropTop->setValidator(new QIntValidator(0, 9999, this));
    ui->editCropRight->setValidator(new QIntValidator(0, 9999, this));
    ui->editCropBottom->setValidator(new QIntValidator(0, 9999, this));

    ui->editResizeWidth->setValidator(new QIntValidator(0, 9999, this));
    ui->editResizeHeight->setValidator(new QIntValidator(0, 9999, this));

    connect(ui->checkBoxResize, SIGNAL(stateChanged(int)), this, SLOT(onResizeChecked(int)));

    _rotateGroup = new QButtonGroup(this);
    _rotateGroup->setExclusive(true);

    _rotateGroup->addButton(ui->radioButtonRotateNone, eTransformRotateType::None);
    _rotateGroup->addButton(ui->radioButtonRotate90CounterClockwise, eTransformRotateType::Deg90CounterClockwise);
    _rotateGroup->addButton(ui->radioButtonRotate180, eTransformRotateType::Deg180);
    _rotateGroup->addButton(ui->radioButtonRotate90Clockwise, eTransformRotateType::Deg90Clockwise);
    _rotateGroup->addButton(ui->radioButtonRotateHorizontal, eTransformRotateType::Horizontal);
    _rotateGroup->addButton(ui->radioButtonRotateURLL, eTransformRotateType::UpperRightDiagonal);
    _rotateGroup->addButton(ui->radioButtonRotateVertical, eTransformRotateType::Vertical);
    _rotateGroup->addButton(ui->radioButtonRotateULLR, eTransformRotateType::UpperLeftDiagonal);

    connect(ui->btnApply, SIGNAL(pressed()), this, SLOT(onApplyPressed()));
    connect(ui->btnCancel, SIGNAL(pressed()), this, SLOT(onCancelPressed()));
}

TransformSettingsDialog::~TransformSettingsDialog()
{
    delete ui;
}

int TransformSettingsDialog::GetCropLeft() const
{
    return ui->editCropLeft->text().toInt();
}

int TransformSettingsDialog::GetCropTop() const
{
    return ui->editCropTop->text().toInt();
}

int TransformSettingsDialog::GetCropRight() const
{
    return ui->editCropRight->text().toInt();
}

int TransformSettingsDialog::GetCropBottom() const
{
    return ui->editCropBottom->text().toInt();
}

bool TransformSettingsDialog::IsResizeActivated()
{
    return ui->checkBoxResize->isChecked();
}

int TransformSettingsDialog::GetResizeWidth()
{
    return ui->editResizeWidth->text().toInt();
}

int TransformSettingsDialog::GetResizeHeight()
{
    return ui->editResizeHeight->text().toInt();
}

eTransformRotateType TransformSettingsDialog::GetRotateType() const
{
    return (eTransformRotateType)_rotateGroup->checkedId();
}

void TransformSettingsDialog::setTransformSettings(const TransformSettings &transformSettings)
{
    ui->editCropLeft->setText(QString::number(transformSettings.cropLeft));
    ui->editCropTop->setText(QString::number(transformSettings.cropTop));
    ui->editCropRight->setText(QString::number(transformSettings.cropRight));
    ui->editCropBottom->setText(QString::number(transformSettings.cropBottom));

    ui->checkBoxResize->setChecked(transformSettings.isResizeActivated);
    ui->editResizeWidth->setText(QString::number(transformSettings.resizeWidth));
    ui->editResizeHeight->setText(QString::number(transformSettings.resizeHeight));

    QRadioButton *pRotateRadioBtn = dynamic_cast<QRadioButton*>(_rotateGroup->button((int)transformSettings.rotateType));
    if (pRotateRadioBtn != nullptr)
    {
        pRotateRadioBtn->setChecked(true);
    }
}

void TransformSettingsDialog::onResizeChecked(int state)
{
    bool enabled = state == Qt::Checked;
    ui->editResizeWidth->setEnabled(enabled);
    ui->editResizeHeight->setEnabled(enabled);
}

void TransformSettingsDialog::onApplyPressed()
{
    emit settingsApplied();
    close();
}

void TransformSettingsDialog::onCancelPressed()
{
    close();
}
