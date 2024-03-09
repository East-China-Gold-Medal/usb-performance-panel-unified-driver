/* @file adjust_dialog.cpp

    Implementation of "Adjust" dialog.
    SPDX-License-Identifier: WTFPL

*/

#include "control_panel.h"
#include "afxdialogex.h"
#include "adjust_dialog.h"
#include <datasource.h>

IMPLEMENT_DYNAMIC(AdjustDialog, CDialogEx)

extern "C" {
    extern uint8_t* calibration_values;
}

AdjustDialog::AdjustDialog(int selectedChannelId, CWnd* pParent /* nullptr */)
    : CDialogEx(IDD_ADJUST_DIALOG, pParent),selectedChannelId(selectedChannelId)
{
    memset(cachedChannel, 0x0, sizeof(cachedChannel));
}

AdjustDialog::~AdjustDialog()
{
}

void AdjustDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_ValueText);
    DDX_Control(pDX, IDC_SLIDER1, m_ValueSlider);
    DDX_Control(pDX, IDC_CHANNEL_COMBO, m_ChannelCombo);
    DDX_Control(pDX, IDC_ADJUST_HINT, m_AdjustHint);
}

BOOL AdjustDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    TCHAR DigitText[MAX_CHANNEL_NAME_LENGTH];
    _tcscpy_s(cachedChannel, MAX_CHANNEL_NAME_LENGTH, get_source_binding_name((uint8_t)selectedChannelId));
    _stprintf_s(DigitText, MAX_CHANNEL_NAME_LENGTH, _T("%d"), calibration_values[(uint8_t)selectedChannelId]);
    clear_binding((uint8_t)selectedChannelId);
    m_ValueText.SetWindowText(DigitText);
    _stprintf_s(DigitText, MAX_CHANNEL_NAME_LENGTH, _T("Channel %d settings"), selectedChannelId);
    SetWindowText(DigitText);
    m_ValueSlider.SetRange(0, 255);
    m_ValueSlider.SetPos(calibration_values[(uint8_t)selectedChannelId]);

    int Index = 0;
    status_t Status = STATUS_HAS_NEXT;
    while (Status == STATUS_HAS_NEXT) {
        Status = iterate_binding_names(DigitText, 32, Index++);
        if (Status != STATUS_HAS_NEXT && Status != STATUS_SUCCESS) {
            break;
        }
        m_ChannelCombo.AddString(DigitText);
    }
    m_ChannelCombo.SelectString(-1, cachedChannel);
    return TRUE;
}

void AdjustDialog::OnCancel()
{
    CDialogEx::OnCancel();
    set_channel_source_binding((uint8_t)selectedChannelId, cachedChannel);
}

BEGIN_MESSAGE_MAP(AdjustDialog, CDialogEx)
    ON_WM_HSCROLL()
    ON_CBN_SELCHANGE(IDC_CHANNEL_COMBO, &AdjustDialog::OnCbnSelchangeChannelCombo)
END_MESSAGE_MAP()

void AdjustDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    UNREFERENCED_PARAMETER(nSBCode);
    UNREFERENCED_PARAMETER(nPos);
    UNREFERENCED_PARAMETER(pScrollBar);
    UpdateData(TRUE);
    CSliderCtrl* pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
    int val = 1 * pSlidCtrl->GetPos();
    TCHAR str[8]{};
    _stprintf_s(str, 8, __TEXT("%d"), val);
    m_ValueText.SetWindowText(str);
    send_usage((uint8_t)selectedChannelId, (uint8_t)val);
    set_channel_calibration((uint8_t)selectedChannelId, (uint8_t)val);
    UpdateData(FALSE);
}

void AdjustDialog::OnCbnSelchangeChannelCombo()
{
    m_ChannelCombo.GetWindowText(cachedChannel, MAX_CHANNEL_NAME_LENGTH);
}
