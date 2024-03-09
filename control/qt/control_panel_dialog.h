/* @file control_panel_dialog.h

    Main entry of Qt-based control panel dialog.
    SPDX-License-Identifier: WTFPL

*/

#ifndef CONTROL_PANEL_DIALOG_H
#define CONTROL_PANEL_DIALOG_H

#include <QDialog>
#include <QMenu>
#include <QTimer>

namespace Ui {
class ControlPanelDialog;
}

class ControlPanelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ControlPanelDialog(QWidget *parent = nullptr);
    ~ControlPanelDialog();
    bool eventFilter(QObject *o, QEvent *event);

private slots:
    void on_button_about_clicked();
    void on_menu_triggered();

private:
    Ui::ControlPanelDialog *ui;
    QMenu *menu;
    QAction *menu_action;
    QTimer t;

};

#endif // CONTROL_PANEL_DIALOG_H
