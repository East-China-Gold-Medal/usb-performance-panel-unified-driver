/* @file adjust_dialog.h

    Main entry of Qt-based adjust dialog.
    SPDX-License-Identifier: WTFPL

*/

#ifndef ADJUST_DIALOG_H
#define ADJUST_DIALOG_H

#include <QDialog>
#include <common.h>

namespace Ui {
class AdjustDialog;
}

class AdjustDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdjustDialog(uint8_t channel, QWidget *parent = nullptr);
    ~AdjustDialog();
    void closeEvent(QCloseEvent *);

private slots:
    void on_slide_calibration_valueChanged(int value);

    void on_combo_channel_currentTextChanged(const QString &arg1);

private:
    Ui::AdjustDialog *ui;
    uint8_t channel;
    char_t cachedChannelName[MAX_CHANNEL_NAME_LENGTH];
};

#endif // ADJUST_DIALOG_H
