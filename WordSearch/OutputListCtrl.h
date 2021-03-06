#pragma once


// COutputListCtrl
class CMainFrame;

class COutputListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(COutputListCtrl)

public:
	COutputListCtrl();
	virtual ~COutputListCtrl();
	CMainFrame* m_pFrame;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


