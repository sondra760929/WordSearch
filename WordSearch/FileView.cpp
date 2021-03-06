
#include "stdafx.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "PDFIndexExplorer.h"
#include "PDFIndexExplorerView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView()
{
}

CFileView::~CFileView()
{
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 메시지 처리기

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE |
		TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS |
		TVS_EDITLABELS | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("파일 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 뷰 이미지를 로드합니다.
	//m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	//m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndFileView.SetFlags(m_wndFileView.GetFlags() | SHCONTF_NONFOLDERS);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 잠금 */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	FillFileView();
	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::FillFileView()
{
	//HTREEITEM hRoot = m_wndFileView.InsertItem(_T("FakeApp 파일"), 0, 0);
	//m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	//HTREEITEM hSrc = m_wndFileView.InsertItem(_T("FakeApp 소스 파일"), 0, 0, hRoot);

	//m_wndFileView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
	//m_wndFileView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
	//m_wndFileView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
	//m_wndFileView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
	//m_wndFileView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	//m_wndFileView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

	//HTREEITEM hInc = m_wndFileView.InsertItem(_T("FakeApp 헤더 파일"), 0, 0, hRoot);

	//m_wndFileView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
	//m_wndFileView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
	//m_wndFileView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
	//m_wndFileView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	//m_wndFileView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	//m_wndFileView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

	//HTREEITEM hRes = m_wndFileView.InsertItem(_T("FakeApp 리소스 파일"), 0, 0, hRoot);

	//m_wndFileView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
	//m_wndFileView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
	//m_wndFileView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
	//m_wndFileView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

	//m_wndFileView.Expand(hRoot, TVE_EXPAND);
	//m_wndFileView.Expand(hSrc, TVE_EXPAND);
	//m_wndFileView.Expand(hInc, TVE_EXPAND);
}

//void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
//{
//	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
//	ASSERT_VALID(pWndTree);
//
//	if (pWnd != pWndTree)
//	{
//		CDockablePane::OnContextMenu(pWnd, point);
//		return;
//	}
//
//	if (point != CPoint(-1, -1))
//	{
//		// 클릭한 항목을 선택합니다.
//		CPoint ptTree = point;
//		pWndTree->ScreenToClient(&ptTree);
//
//		UINT flags = 0;
//		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
//		if (hTreeItem != nullptr)
//		{
//			pWndTree->SelectItem(hTreeItem);
//		}
//	}
//
//	pWndTree->SetFocus();
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
//}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = 0;// m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnProperties()
{
	AfxMessageBox(_T("속성...."));

}

void CFileView::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnFileOpenWith()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnDummyCompile()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnEditCut()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnEditCopy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnEditClear()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();

	CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
	if (frame)
	{
		CPDFIndexExplorerView* pview = (CPDFIndexExplorerView*)(frame->GetActiveView());
		if (pview)
		{
			pview->KillTimer(2);
			pview->KillTimer(3);
		}
	}

}

void CFileView::OnChangeVisualStyle()
{
	//m_wndToolBar.CleanUpLockedImages();
	//m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 잠금 */);

	//m_FileViewImages.DeleteImageList();

	//UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	//CBitmap bmp;
	//if (!bmp.LoadBitmap(uiBmpId))
	//{
	//	TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
	//	ASSERT(FALSE);
	//	return;
	//}

	//BITMAP bmpObj;
	//bmp.GetBitmap(&bmpObj);

	//UINT nFlags = ILC_MASK;

	//nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	//m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	//m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	//m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}

//BOOL CFileView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	if (wParam == 4)
//	{
//		LPNMHDR lpnmh = (LPNMHDR)lParam;
//		ENSURE(lpnmh != NULL);
//
//		if (lpnmh->code == TVN_SELCHANGED)
//		{
//			CString file_path;
//			if (m_wndFileView.GetItemPath(file_path, m_wndFileView.GetSelectedItem()))
//			{
//				CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
//				if (frame)
//				{
//					CPDFIndexExplorerView* pview = (CPDFIndexExplorerView*)(frame->GetActiveView());
//					if (pview)
//					{
//						pview->SelectFileFromTree(file_path);
//					}
//				}
//			}
//			//CMFCShellListCtrl* pRelatedShellList = GetRelatedList();
//
//			//if (pRelatedShellList != NULL && GetSelectedItem() != NULL)
//			//{
//			//	ASSERT_VALID(pRelatedShellList);
//			//	LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO)GetItemData(GetSelectedItem());
//
//			//	pRelatedShellList->m_bNoNotify = TRUE;
//			//	pRelatedShellList->DisplayFolder(pItem);
//			//	pRelatedShellList->m_bNoNotify = FALSE;
//
//			//	return TRUE;
//			//}
//		}
//	}
//
//	return CDockablePane::OnNotify(wParam, lParam, pResult);
//}


BOOL CFileView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// Check whether its enter key. 
		if (pMsg->wParam == VK_RETURN)
		{
			// Get the edit control. 
			CEdit* pEdit = m_wndFileView.GetEditControl();

			// If Edit control is null, then user is not  
			// editing the label of tree control. 
			if (pEdit)
			{
//				m_wndFileView.canUpdateLabel = true;
				//CString file_name;
				//CString file_path = m_wndFileView.m_strCurrentPath;
				//if (PathFileExists(file_path))
				//{
				//	CString new_file_path;
				//	char drive[_MAX_DRIVE];
				//	char dir[_MAX_DIR];
				//	char fname[_MAX_FNAME];
				//	char ext[_MAX_EXT];
				//	_splitpath_s(file_path, drive, sizeof drive, dir, sizeof dir, fname, sizeof fname, ext, sizeof ext);

				//	new_file_path.Format("%s%s%s%s", drive, dir, file_name, ext);

				//	if (MoveFile(file_path, new_file_path))
				//	{
				//		m_wndFileView.Refresh();
				//		m_wndFileView.SelectPath(new_file_path);
				//	}
				//}
				// Send Mouse LeftButton click to end edit. 
				// Use PostMessage since its async and won't  
				// distrub PreTranslateMessage() flow. 
				m_wndFileView.PostMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(-1, -1));
			}
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			// Get the edit control. 
			CEdit* pEdit = m_wndFileView.GetEditControl();

			// If Edit control is null, then user is not  
			// editing the label of tree control. 
			if (pEdit)
			{
//				m_wndFileView.canUpdateLabel = false;
				// Send Mouse LeftButton click to end edit. 
				// Use PostMessage since its async and won't  
				// distrub PreTranslateMessage() flow. 
				m_wndFileView.PostMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(-1, -1));
			}
		}
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}

void CFileView::SetFolder(CString folder_path)
{
	m_wndFileView.Refresh();
	m_wndFileView.SelectPath(folder_path);
}

