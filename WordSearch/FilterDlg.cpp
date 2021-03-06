// FilterDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "PDFIndexExplorer.h"
#include "FilterDlg.h"
#include "afxdialogex.h"


// CFilterDlg 대화 상자

IMPLEMENT_DYNAMIC(CFilterDlg, CDialog)

CFilterDlg::CFilterDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_FILTER, pParent)
{
	m_mapFilter = NULL;
}

CFilterDlg::~CFilterDlg()
{
}

void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILTER, m_ctrlFilter);
}


BEGIN_MESSAGE_MAP(CFilterDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILTER, &CFilterDlg::OnNMClickListFilter)
END_MESSAGE_MAP()


// CFilterDlg 메시지 처리기


void CFilterDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (GetDlgItem(IDOK))
	{
		int offset = 2;
		int height = 30;
		m_ctrlFilter.MoveWindow(offset, offset, cx - offset - offset, cy - offset - offset - offset - height, TRUE);
		GetDlgItem(IDOK)->MoveWindow(offset, cy - offset - height, cx - offset - offset, height, TRUE);
	}
}


BOOL CFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlFilter.SetExtendedStyle(LVS_EX_CHECKBOXES);
	MoveWindow(m_rect.left, m_rect.top, m_rect.Width(), 200);
	if (m_mapFilter)
	{
		int index = 0;
		bool is_all = true;
		auto itr = m_mapFilter->begin();
		while (itr != m_mapFilter->end())
		{
			index = m_ctrlFilter.InsertItem(index + 1, itr->first);
			m_ctrlFilter.SetCheck(index, itr->second);
			if (itr->second == false)
			{
				is_all = false;
			}
			itr++;
		}
		index = m_ctrlFilter.InsertItem(0, "All");
		m_ctrlFilter.SetCheck(index, is_all);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CFilterDlg::OnNMClickListFilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	LVHITTESTINFO info;
	info.pt = pNMListView->ptAction;
	m_ctrlFilter.HitTest(&info);
	
	if (info.flags == LVHT_ONITEMSTATEICON) 
	{
		// 체크박스가 클릭되면 여기로 들어온다.
		CString item_text = m_ctrlFilter.GetItemText(pNMListView->iItem, 0);
		// 여기서 체크박스 상태를 확인 후 처리를 추가하면된다.
		if (m_ctrlFilter.GetCheck(pNMListView->iItem)) {
			// 체크된 상태일 때 처리
			if (item_text == "All")
			{
				for (int i = 1; i < m_ctrlFilter.GetItemCount(); i++)
				{
					m_ctrlFilter.SetCheck(i, FALSE);
				}
				auto itr = m_mapFilter->begin();
				while (itr != m_mapFilter->end())
				{
					itr->second = false;
					itr++;
				}
			}
			else
			{
				(*m_mapFilter)[item_text] = false;
				m_ctrlFilter.SetCheck(0, FALSE);
			}
		}
		else {

			// 체크되지 않은 상태일 때 처리
			if (item_text == "All")
			{
				for (int i = 1; i < m_ctrlFilter.GetItemCount(); i++)
				{
					m_ctrlFilter.SetCheck(i, TRUE);
				}
				auto itr = m_mapFilter->begin();
				while (itr != m_mapFilter->end())
				{
					itr->second = true;
					itr++;
				}
			}
			else
			{
				(*m_mapFilter)[item_text] = true;
			}
		}
	}
	else
	{
		// 체크박스가 클릭되면 여기로 들어온다.
		CString item_text = m_ctrlFilter.GetItemText(pNMListView->iItem, 0);
		// 여기서 체크박스 상태를 확인 후 처리를 추가하면된다.
		if (m_ctrlFilter.GetCheck(pNMListView->iItem)) {
			// 체크된 상태일 때 처리
			if (item_text == "All")
			{
				for (int i = 0; i < m_ctrlFilter.GetItemCount(); i++)
				{
					m_ctrlFilter.SetCheck(i, FALSE);
				}
				auto itr = m_mapFilter->begin();
				while (itr != m_mapFilter->end())
				{
					itr->second = false;
					itr++;
				}
			}
			else
			{
				(*m_mapFilter)[item_text] = false;
				m_ctrlFilter.SetCheck(pNMListView->iItem, FALSE);
				m_ctrlFilter.SetCheck(0, FALSE);
			}
		}
		else {

			// 체크되지 않은 상태일 때 처리
			if (item_text == "All")
			{
				for (int i = 0; i < m_ctrlFilter.GetItemCount(); i++)
				{
					m_ctrlFilter.SetCheck(i, TRUE);
				}
				auto itr = m_mapFilter->begin();
				while (itr != m_mapFilter->end())
				{
					itr->second = true;
					itr++;
				}
			}
			else
			{
				(*m_mapFilter)[item_text] = true;
				m_ctrlFilter.SetCheck(pNMListView->iItem, TRUE);
			}
		}
	}

	*pResult = 0;
}
