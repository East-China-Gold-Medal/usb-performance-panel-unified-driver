/* @file about_dialog.h

    Definition of "About" dialog.
    SPDX-License-Identifier: WTFPL

*/

#pragma once
#include "afxdialogex.h"

class AboutDialog : public CDialogEx
{
    DECLARE_DYNAMIC(AboutDialog)

public:
    AboutDialog(CWnd* pParent = nullptr);
    virtual ~AboutDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUT_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    CStatic m_img;
    CStatic m_desc;
    DECLARE_MESSAGE_MAP()
};
