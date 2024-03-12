/* @file control_panel_dialog.cpp

    Implementation of "Control Panel" dialog.
    SPDX-License-Identifier: WTFPL

*/

#include <datasource.h>
#include "framework.h"
#include "control_panel.h"
#include "control_panel_dialog.h"
#include "afxdialogex.h"
#include "about_dialog.h"
#include "adjust_dialog.h"

#define WM_NC 666

// For modern UI.
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

extern "C" {
    extern uint8_t* calibration_values;
}

ControlPanelDialog::ControlPanelDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CONTROL_PANEL_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    memset(&NotifyIcon, 0, sizeof(NotifyIcon));
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
    ON_WM_SYSCOMMAND()
    ON_WM_CLOSE()
    ON_MESSAGE(WM_NC, &ControlPanelDialog::OnNotifyIcon)
    ON_NOTIFY(NM_RCLICK, IDC_PANEL_LIST, &ControlPanelDialog::OnPanelListRightClick)
    ON_NOTIFY(NM_DBLCLK, IDC_PANEL_LIST, &ControlPanelDialog::OnPanelListDblClick)
    ON_BN_CLICKED(IDC_ABOUT_BUTTON, &ControlPanelDialog::OnBnClickedAboutButton)
    ON_COMMAND(ID_MENU01_CALIBRATE, &ControlPanelDialog::OnMenu01Calibrate)
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
    m_OverallDesc.SetWindowText(Buffer);

    for (int i = 0; i < channel_count; ++i) {
        _stprintf_s(Buffer, 128, _T("%d"), i + 1);
        m_PanelList.InsertItem(i, Buffer);
        m_PanelList.SetItemText(i, 1, get_source_binding_name((uint8_t)i));
        _stprintf_s(Buffer, 128, _T("%d(0x%x)"), calibration_values[i], calibration_values[i]);
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
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if (pNMListView->iItem != -1) {
        CPoint pt;
        GetPhysicalCursorPos(&pt);
        CMenu menu;
        menu.LoadMenu(ID_LIST_RIGHTCLICK_MENU);
        CMenu* popmenu;
        popmenu = menu.GetSubMenu(0);
        popmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
    }
    *pResult = 0;
}

void ControlPanelDialog::OnPanelListDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNMHDR);
    OnMenu01Calibrate();
    *pResult = 0;
}

void ControlPanelDialog::OnBnClickedAboutButton()
{
    AboutDialog about;
    about.DoModal();
}

void ControlPanelDialog::OnMenu01Calibrate()
{
    TCHAR Buffer[32];
    POSITION p = m_PanelList.GetFirstSelectedItemPosition();
    if (!p) {
        return;
    }
    uint8_t index = (uint8_t)m_PanelList.GetNextSelectedItem(p);
    AdjustDialog dlg(index, this);
    dlg.DoModal();
    _stprintf_s(Buffer, 32, _T("%d(0x%x)"), calibration_values[index], calibration_values[index]);
    dlg.DestroyWindow();
    m_PanelList.SetItemText(index, 2, Buffer);
    m_PanelList.SetItemText(index, 1, get_source_binding_name(index));
}

void ControlPanelDialog::MinimizeToTray()
{
    ShowWindow(SW_HIDE);
    NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
    NotifyIcon.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    NotifyIcon.hWnd = m_hWnd;
    lstrcpy(NotifyIcon.szTip, __TEXT("ECGM USB Perf Monitor Control"));
    NotifyIcon.uCallbackMessage = WM_NC;
    NotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    Shell_NotifyIcon(NIM_ADD, &NotifyIcon);
}

void ControlPanelDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SC_MINIMIZE) {
        MinimizeToTray();
    }
    else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

LRESULT ControlPanelDialog::OnNotifyIcon(WPARAM wParam, LPARAM IParam)
{
    if ((IParam == WM_LBUTTONDOWN) || (IParam == WM_RBUTTONDOWN))
    {
        ModifyStyleEx(0, WS_EX_TOPMOST);
        ShowWindow(SW_SHOW);
    }
    UNREFERENCED_PARAMETER(wParam);
    return 0;
}


void ControlPanelDialog::OnClose()
{
    Shell_NotifyIcon(NIM_DELETE, &NotifyIcon);
    CDialogEx::OnClose();
}