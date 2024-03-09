/* @file about_dialog.cpp

    Main entry of Qt-based about dialog.
    SPDX-License-Identifier: WTFPL

*/

#include "about_dialog.h"
#include "ui_about_dialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~(Qt::WindowMinMaxButtonsHint));
    setFixedSize (this->width (), this->height ());
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_clicked()
{
    close();
}

