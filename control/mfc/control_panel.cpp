/* @file control_panel.cpp

	Main entry of control panel application.
	SPDX-License-Identifier: WTFPL

*/

#include <platform.h>
#include <datasource.h>
#include "framework.h"
#include "control_panel.h"
#include "control_panel_dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

uint8_t channel_count;

// CcontrolPanelApp

BEGIN_MESSAGE_MAP(CcontrolPanelApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CcontrolPanelApp::CcontrolPanelApp()
{
	open_device();
	channel_count = get_channel_count();
}

CcontrolPanelApp theApp;


BOOL CcontrolPanelApp::InitInstance()
{
	CWinApp::InitInstance();

	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("EastChinaGoldMedalControlPanel"));

	ControlPanelDialog dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) {

	}
	else if (nResponse == IDCANCEL) {

	}
	else if (nResponse == -1) {
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	if (pShellManager != nullptr) {
		delete pShellManager;
	}
#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;
}