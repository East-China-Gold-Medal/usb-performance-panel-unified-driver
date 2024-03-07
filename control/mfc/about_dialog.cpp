/* @file about_dialog.cpp

	Implementation of "About" dialog.
	SPDX-License-Identifier: WTFPL

*/

#include "control_panel.h"
#include "afxdialogex.h"
#include "about_dialog.h"

IMPLEMENT_DYNAMIC(AboutDialog, CDialogEx)

AboutDialog::AboutDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ABOUT_DIALOG, pParent)
{

}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AboutDialog, CDialogEx)
END_MESSAGE_MAP()
