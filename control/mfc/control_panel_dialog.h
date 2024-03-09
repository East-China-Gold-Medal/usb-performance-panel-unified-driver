/* @file control_panel_dialog.h

    Definition of "Control Panel" dialog.
    SPDX-License-Identifier: WTFPL

*/

#pragma once

class ControlPanelDialog : public CDialogEx
{
public:
    ControlPanelDialog(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MFCTEST_DIALOG };
#endif

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    HICON m_hIcon;
    CListCtrl m_PanelList;
    CStatic m_OverallDesc;
    NOTIFYICONDATA NotifyIcon;

    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnPanelListRightClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnPanelListDblClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedAboutButton();
    afx_msg void OnMenu01Calibrate();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM IParam);
    afx_msg void OnClose();
    void MinimizeToTray();
    DECLARE_MESSAGE_MAP()
};

EXTERN_C_START
extern uint8_t channel_count;
EXTERN_C_END