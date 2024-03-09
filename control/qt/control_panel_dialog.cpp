/* @file control_panel_dialog.cpp

    Main entry of Qt-based control panel dialog.
    SPDX-License-Identifier: WTFPL

*/

#include "control_panel_dialog.h"
#include "ui_control_panel_dialog.h"
#include <common.h>
#include <datasource.h>
#include "about_dialog.h"
#include "adjust_dialog.h"

EXTERN_C_START
    extern uint8_t channel_count;
    extern uint8_t* calibration_values;
    extern status_t main_loop_callback(void);
EXTERN_C_END

ControlPanelDialog::ControlPanelDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ControlPanelDialog)
{
    ui->setupUi(this);
    ui->label_summary->setText(
        QString("Your East China Gold Medal Performance Panel supports up to %1 channels.")
            .arg(channel_count)
    );
    ui->panel_table->horizontalHeader()->setStretchLastSection(true);
    ui->panel_table->setHorizontalHeaderLabels( {
        "Panel ID",
        "Channel",
        "Calibration"
    });
    ui->panel_table->setRowCount(channel_count);
    for(uint8_t i=0;i<channel_count;i++) {
        ui->panel_table->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));
        ui->panel_table->setItem(i,1,new QTableWidgetItem(get_source_binding_name(i)));
        ui->panel_table->setItem(i,2,new QTableWidgetItem(QString::number(calibration_values[i])));
    }
    menu = new QMenu(this);
    menu_action = new QAction("Calibrate......",this);
    menu->addAction(menu_action);
    connect(menu_action, SIGNAL(triggered()), this, SLOT(on_menu_triggered()));
    ui->panel_table->installEventFilter(this);
    t.setInterval(LOOP_INTERVAL_MS);
    connect(&t,&QTimer::timeout,this,[=]()
            {
                main_loop_callback();
            });
    t.start();
}

ControlPanelDialog::~ControlPanelDialog()
{
    delete ui;
    t.stop();
    for(uint8_t i=0;i<channel_count;i++) {
        send_usage(i, 0);
    }
}

void ControlPanelDialog::on_button_about_clicked()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->open();
}

bool ControlPanelDialog::eventFilter(QObject *o, QEvent *event)
{
    if (o == ui->panel_table) {
        if (event->type() == QEvent::ContextMenu) {
            menu->exec(QCursor::pos());
            return true;
        }
    }
    return QObject::eventFilter(o,event);
}

void ControlPanelDialog::on_menu_triggered()
{
    int row = ui->panel_table->currentRow();
    if(row < 0) {
        return;
    }
    AdjustDialog adjust(row, this);
    adjust.exec();
    ui->panel_table->setItem(row,1,new QTableWidgetItem(get_source_binding_name(row)));
    ui->panel_table->setItem(row,2,new QTableWidgetItem(QString::number(calibration_values[row])));
}
