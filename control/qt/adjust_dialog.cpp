/* @file control_panel_dialog.cpp

    Main entry of Qt-based adjust dialog.
    SPDX-License-Identifier: WTFPL

*/

#include "adjust_dialog.h"
#include "ui_adjust_dialog.h"
#include <datasource.h>
#include <QCloseEvent>

extern "C" {
    extern uint8_t* calibration_values;
}

void AdjustDialog::closeEvent(QCloseEvent * event)
{
    set_channel_source_binding(channel, cachedChannelName);
    event->accept();
}

AdjustDialog::AdjustDialog(uint8_t channel, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AdjustDialog), channel(channel)
{
    status_t status = STATUS_HAS_NEXT;
    char_t buf[MAX_CHANNEL_NAME_LENGTH];
    int index=0;

    ui->setupUi(this);
    setWindowTitle(QString("Channel %1 settings").arg(channel));
    ui->slide_calibration->setSliderPosition(calibration_values[channel]);
    ui->text_calibration->setText(QString::number(calibration_values[channel]));
    strcpy(cachedChannelName, get_source_binding_name((uint8_t)channel));
    clear_binding(channel);
    send_usage(channel, calibration_values[channel]);

    while (status == STATUS_HAS_NEXT) {
        status = iterate_binding_names(buf, MAX_CHANNEL_NAME_LENGTH, index++);
        if (status != STATUS_HAS_NEXT && status != STATUS_SUCCESS) {
            break;
        }
        ui->combo_channel->addItem(buf);
    }

    ui->combo_channel->setCurrentText(cachedChannelName);
}

AdjustDialog::~AdjustDialog()
{
    delete ui;
}

void AdjustDialog::on_slide_calibration_valueChanged(int value)
{
    send_usage(channel, (uint8_t)value);
    set_channel_calibration(channel, (uint8_t)value);
    ui->text_calibration->setText(QString::number(calibration_values[channel]));
}


void AdjustDialog::on_combo_channel_currentTextChanged(const QString &arg1)
{
    if(!isVisible()) { // During init.
        return;
    }
    strcpy(cachedChannelName,arg1.toLocal8Bit());
}

