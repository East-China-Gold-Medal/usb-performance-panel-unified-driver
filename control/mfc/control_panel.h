/* @file control_panel.h

    Main entry class of control panel application.
    SPDX-License-Identifier: WTFPL

*/

#pragma once

#include "framework.h"
#include "resource.h"

class CcontrolPanelApp : public CWinApp
{
public:
    CcontrolPanelApp();
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CcontrolPanelApp theApp;
