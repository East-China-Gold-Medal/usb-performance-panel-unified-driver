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

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPanelListRightClick(NMHDR* pNMHDR, LRESULT* pResult);
};

EXTERN_C_START
extern uint8_t channel_count;
EXTERN_C_END