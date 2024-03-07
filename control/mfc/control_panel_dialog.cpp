/* @file control_panel_dialog.cpp

	Implementation of "Control Panel" dialog.
	SPDX-License-Identifier: WTFPL

*/

#include <datasource.h>
#include "framework.h"
#include "control_panel.h"
#include "control_panel_dialog.h"
#include "afxdialogex.h"

// For modern UI.
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

ControlPanelDialog::ControlPanelDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONTROL_PANEL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ControlPanelDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PANEL_LIST, m_PanelList);
	DDX_Control(pDX, IDC_OVERALL_DESC, m_OverallDesc);
}

BEGIN_MESSAGE_MAP(ControlPanelDialog, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK, IDC_PANEL_LIST, &ControlPanelDialog::OnPanelListRightClick)
END_MESSAGE_MAP()

BOOL ControlPanelDialog::OnInitDialog()
{
	TCHAR Buffer[128];
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	SetWindowText(L"ECGM USB Perf Panel Control");

	CRect PanelListSize;
	m_PanelList.GetClientRect(&PanelListSize);
	m_PanelList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_PanelList.InsertColumn(0, _T("Panel ID"), LVCFMT_LEFT, PanelListSize.Width() / 3);
	m_PanelList.InsertColumn(1, _T("Channel"), LVCFMT_LEFT, PanelListSize.Width() / 3);
	m_PanelList.InsertColumn(2, _T("Calibration"), LVCFMT_LEFT, PanelListSize.Width() / 3);

	_stprintf_s(Buffer, 128, _T("Your East China Gold Medal Control Panel supports up to %d channels."), channel_count);
	m_OverallDesc.SetWindowTextW(Buffer);

	for (int i = 0; i < channel_count; ++i) {
		_stprintf_s(Buffer, 128, _T("%d"), i + 1);
		m_PanelList.InsertItem(i, Buffer);
		m_PanelList.SetItemText(i, 1, Buffer);
		m_PanelList.SetItemText(i, 2, Buffer);
	}
	m_PanelList.ModifyStyle(0, LVS_SINGLESEL);
	m_PanelList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |
#if WINVER >= _WIN32_WINNT_VISTA
								 LVS_EX_JUSTIFYCOLUMNS | 
#endif
								 LVS_EX_DOUBLEBUFFER);
#if WINVER >= _WIN32_WINNT_WS03
	SetWindowTheme(m_PanelList, L"Explorer", NULL);
#endif
	return TRUE;
}

void ControlPanelDialog::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialogEx::OnPaint();
	}
}

HCURSOR ControlPanelDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void ControlPanelDialog::OnPanelListRightClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1) {
		CPoint pt;
		GetCursorPos(&pt);
		CMenu menu;
		menu.LoadMenu(ID_LIST_RIGHTCLICK_MENU);
		CMenu* popmenu;
		popmenu = menu.GetSubMenu(0);
		popmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}
	*pResult = 0;
}
