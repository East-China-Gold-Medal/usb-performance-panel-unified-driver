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

static const TCHAR* desc = __TEXT(
    "A USB Performance Panel by East China Gold Medal team.\r\n"
    "East China Gold Medal is an interest group from Hangzhou Dianzi University Information Engineering College (HDUIEC).\r\n"
    "Developers:\r\n"
    "Igniculus Fu(@fhh200000)\r\n"
    "Avanter Chu(@Avanter98)\r\n"
    "Vanilla Chen(@Vanilla-Cbc)");

void AboutDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, ID_ECGM_LOGO, m_img);
    DDX_Control(pDX, IDC_DESC, m_desc);
    CRect rect;
    GetClientRect(&rect);
    int dpi = GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSX);
    HICON Icon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, dpi, dpi, LR_DEFAULTCOLOR);
    m_img.SetIcon(Icon);
    m_desc.SetWindowText(desc);
}


BEGIN_MESSAGE_MAP(AboutDialog, CDialogEx)
END_MESSAGE_MAP()
