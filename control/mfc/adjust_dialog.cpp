/* @file adjust_dialog.cpp

	Implementation of "Adjust" dialog.
	SPDX-License-Identifier: WTFPL

*/

#include "control_panel.h"
#include "afxdialogex.h"
#include "adjust_dialog.h"

IMPLEMENT_DYNAMIC(AdjustDialog, CDialogEx)

AdjustDialog::AdjustDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADJUST_DIALOG, pParent)
{

}

AdjustDialog::~AdjustDialog()
{
}

void AdjustDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ValueText);
	DDX_Control(pDX, IDC_SLIDER1, m_ValueSlider);
}


BEGIN_MESSAGE_MAP(AdjustDialog, CDialogEx)
END_MESSAGE_MAP()
