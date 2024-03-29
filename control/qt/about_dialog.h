/* @file about_dialog.h

    Main entry of Qt-based about dialog.
    SPDX-License-Identifier: WTFPL

*/

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUT_DIALOG_H
