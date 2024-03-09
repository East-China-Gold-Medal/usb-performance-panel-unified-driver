/* @file adjust_dialog.h

    Definition of "Adjust" dialog.
    SPDX-License-Identifier: WTFPL

*/

#pragma once
#include "afxdialogex.h"
#include <common.h>

class AdjustDialog : public CDialogEx
{
    DECLARE_DYNAMIC(AdjustDialog)

public:
    AdjustDialog(int selectedChannelId, CWnd* pParent = nullptr);
    virtual ~AdjustDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ADJUST_DIALOG };
#endif

private:
    int selectedChannelId;
    TCHAR cachedChannel[MAX_CHANNEL_NAME_LENGTH];

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    virtual BOOL OnInitDialog();
    virtual void OnCancel();

    DECLARE_MESSAGE_MAP()
    CEdit m_ValueText;
    CSliderCtrl m_ValueSlider;
    CStatic m_AdjustHint;
    CComboBox m_ChannelCombo;

public:
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnCbnSelchangeChannelCombo();
};
