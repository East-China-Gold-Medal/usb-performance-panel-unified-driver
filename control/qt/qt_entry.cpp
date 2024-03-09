/* @file qt_entry.cpp

    Main entry of Qt-based control application.
    SPDX-License-Identifier: WTFPL

*/

#include <QApplication>
#include <platform.h>
#include <datasource.h>
#include "control_panel_dialog.h"
#include <QMessageBox>
#include <QTimer>

EXTERN_C_START
    uint8_t channel_count;
EXTERN_C_END

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    status_t status = STATUS_SUCCESS;
    status = open_device();
    if(status != STATUS_SUCCESS) {
        QMessageBox::critical(nullptr,"Fatal error","Cannot initialize USB environment!");
        return -1;
    }
    channel_count = get_channel_count();
    status = initialize_data_source_binding(channel_count);
    if(status != STATUS_SUCCESS) {
        QMessageBox::critical(nullptr,"Fatal error","Cannot load channel settings!");
        return -1;
    }
    ControlPanelDialog d;
    d.show();
    return a.exec();
}
