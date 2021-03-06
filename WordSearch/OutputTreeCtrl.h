#pragma once


// COutputTreeCtrl

class COutputTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(COutputTreeCtrl)

public:
	COutputTreeCtrl();
	virtual ~COutputTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};


