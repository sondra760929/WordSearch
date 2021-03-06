// OutputTreeCtrl.cpp: 구현 파일
//

#include "stdafx.h"
#include "PDFIndexExplorer.h"
#include "OutputTreeCtrl.h"
#include "MainFrm.h"
#include "PDFIndexExplorerView.h"


// COutputTreeCtrl

IMPLEMENT_DYNAMIC(COutputTreeCtrl, CTreeCtrl)

COutputTreeCtrl::COutputTreeCtrl()
{

}

COutputTreeCtrl::~COutputTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(COutputTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMCHANGED, &COutputTreeCtrl::OnTvnItemChanged)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &COutputTreeCtrl::OnTvnSelchanged)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_NOTIFY_REFLECT(NM_DBLCLK, &COutputTreeCtrl::OnNMDblclk)
END_MESSAGE_MAP()



// COutputTreeCtrl 메시지 처리기




void COutputTreeCtrl::OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);
	
	*pResult = 0;
}


void COutputTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM current_item = GetSelectedItem();
	if (current_item)
	{
		HTREEITEM parent_item = GetParentItem(current_item);
		if (parent_item)
		{
			CMainFrame* m_pFrame = (CMainFrame*)AfxGetMainWnd();

			if (m_pFrame)
			{
				CPDFIndexExplorerView* pview = (CPDFIndexExplorerView*)(m_pFrame->GetActiveView());
				if (pview)
				{
					pview->SelectResultFromTree(parent_item, current_item);
				}
			}
		}
	}
	*pResult = 0;
}


int COutputTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	long style = GetWindowLong(this->GetSafeHwnd(), GWL_STYLE) | TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS | TVS_LINESATROOT;

	SetWindowLong(this->GetSafeHwnd(), GWL_STYLE, style);

	return 0;
}


void COutputTreeCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CTreeCtrl::OnSetFocus(pOldWnd);

	CMainFrame*	m_pFrame = (CMainFrame*)AfxGetMainWnd();

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

void COutputTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM current_item = GetSelectedItem();
	if (current_item)
	{
		HTREEITEM parent_item = GetParentItem(current_item);
		if (parent_item)
		{
			CMainFrame* m_pFrame = (CMainFrame*)AfxGetMainWnd();

			if (m_pFrame)
			{
				CString file_path = m_pFrame->m_htreeitem_to_fname[parent_item];
				ShellExecute(m_hWnd, "open", file_path, nullptr, nullptr, SW_SHOW);
			}
		}
	}
	*pResult = 0;
}
