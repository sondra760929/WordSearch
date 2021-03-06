// CInputDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "PDFIndexExplorer.h"
#include "CInputDlg.h"
#include "afxdialogex.h"


// CInputDlg 대화 상자

IMPLEMENT_DYNAMIC(CInputDlg, CDialog)

CInputDlg::CInputDlg(CWnd* pParent, CString _title, CString _caption)
	: CDialog(IDD_DIALOG_INPUT, pParent)
	, m_strCaption(_caption)
	, m_strTitle(_title)
	, m_strInput(_T(""))
{

}

CInputDlg::~CInputDlg()
{
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_CAPTION, m_strCaption);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_strInput);
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
END_MESSAGE_MAP()


// CInputDlg 메시지 처리기


BOOL CInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_strTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
