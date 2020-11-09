#include "VideoQualityDialog.h"
#include "ui_VideoQualityDialog.h"

#include <QButtonGroup>

VideoQualityDialog::VideoQualityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VideoQualityDialog)
{
    ui->setupUi(this);

    setWindowTitle("Select quality");
    setFixedSize(150, 70);

    _radioGroup = new QButtonGroup(this);
    _radioGroup->setExclusive(true);

    _radioGroup->addButton(ui->radioButtonLow, 0);
    _radioGroup->addButton(ui->radioButtonHigh, 1);

    ui->radioButtonLow->setChecked(true);

    connect(_radioGroup, SIGNAL(buttonClicked(int)), this, SLOT(onQualitySelected(int)));
}

VideoQualityDialog::~VideoQualityDialog()
{
    delete ui;
}

void VideoQualityDialog::reset()
{
    // emulate default (low) quality selection
    ui->radioButtonLow->setChecked(true);
    emit lowQualitySelected();
}

void VideoQualityDialog::onQualitySelected(int optionId)
{
    switch (optionId)
    {
    case eQuality::Low:
        emit lowQualitySelected();
        break;

    case eQuality::High:
        emit highQualitySelected();
        break;

    default:
        break;
    }

    close();
}
