
#pragma once
#include "OutputListCtrl.h"
#include "OutputTreeCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// COutputList 창

class COutputList : public CListBox
{
// 생성입니다.
public:
	COutputList();

// 구현입니다.
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchange();
};

class COutputWnd : public CDockablePane
{
// 생성입니다.
public:
	COutputWnd();

	void UpdateFonts();
	COutputTreeCtrl m_wndOutputFind;

// 특성입니다.
protected:
	//CMFCTabCtrl	m_wndTabs;

	//COutputList m_wndOutputBuild;
	//COutputList m_wndOutputDebug;

protected:
	//void FillBuildWindow();
	//void FillDebugWindow();
	void FillFindWindow();

	void AdjustHorzScroll(CListBox& wndListBox);

// 구현입니다.
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

