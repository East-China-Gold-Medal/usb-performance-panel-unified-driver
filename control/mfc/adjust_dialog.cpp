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
	: CDialogEx(IDD_ADJUST_DIALOG, pParent),selectedChannelId(selectedChannelId),cachedChannel(nullptr)
{

}

AdjustDialog::~AdjustDialog()
{
	set_channel_source_binding((uint8_t)selectedChannelId, cachedChannel);
}

void AdjustDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ValueText);
	DDX_Control(pDX, IDC_SLIDER1, m_ValueSlider);
}

BOOL AdjustDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	TCHAR DigitText[8];
	cachedChannel = get_source_binding_name((uint8_t)selectedChannelId);
	_stprintf_s(DigitText, 8, _T("%d"), calibration_values[(uint8_t)selectedChannelId]);
	clear_binding((uint8_t)selectedChannelId);
	m_ValueText.SetWindowText(DigitText);
	m_ValueSlider.SetRange(0, 255);
	m_ValueSlider.SetPos(calibration_values[(uint8_t)selectedChannelId]);
	return TRUE;
}

BEGIN_MESSAGE_MAP(AdjustDialog, CDialogEx)
	ON_WM_HSCROLL()
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
