// OutputListCtrl.cpp: 구현 파일
//

#include "stdafx.h"
#include "PDFIndexExplorer.h"
#include "OutputListCtrl.h"
#include "MainFrm.h"
#include "PDFIndexExplorerView.h"

// COutputListCtrl

IMPLEMENT_DYNAMIC(COutputListCtrl, CListCtrl)

COutputListCtrl::COutputListCtrl()
{

}

COutputListCtrl::~COutputListCtrl()
{
}


BEGIN_MESSAGE_MAP(COutputListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_CLICK, &COutputListCtrl::OnNMClick)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// COutputListCtrl 메시지 처리기




int COutputListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	InsertColumn(0, "No", LVCFMT_CENTER, 100);
	InsertColumn(1, "Path", LVCFMT_LEFT, 500);
	InsertColumn(2, "Page", LVCFMT_LEFT, 50);
	InsertColumn(3, "Index", LVCFMT_LEFT, 50);

	return 0;
}


void COutputListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
		AfxMessageBox("No items selected!");
	else
	{
		while (pos)
		{
			int index = GetNextSelectedItem(pos);
			if (index > -1)
			{
				if(!m_pFrame)
					m_pFrame = (CMainFrame*)AfxGetMainWnd();

				if (m_pFrame)
				{
					CPDFIndexExplorerView* pview = (CPDFIndexExplorerView*)(m_pFrame->GetActiveView());
					if (pview)
					{
						CString file_path = GetItemText(index, 1);
						CString page = GetItemText(index, 2);

						pview->SelectResultFromTree(file_path, atoi(page));
					}
				}
			}
		}
	}
	*pResult = 0;
}


void COutputListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);

	if (!m_pFrame)
		m_pFrame = (CMainFrame*)AfxGetMainWnd();

	if (m_pFrame)
	{
		CPDFIndexExplorerView* pview = (CPDFIndexExplorerView*)(m_pFrame->GetActiveView());
		if (pview)
		{
			pview->KillTimer(1);
			pview->KillTimer(2);
		}
	}
}
