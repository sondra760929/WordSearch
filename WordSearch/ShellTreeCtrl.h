#pragma once
#include "MFCShellTreeCtrlEx.h"

// CShellTreeCtrl

class CShellTreeCtrl : public CMFCShellTreeCtrl
{
	DECLARE_DYNAMIC(CShellTreeCtrl)

public:
	CShellTreeCtrl();
	virtual ~CShellTreeCtrl();
	HRESULT EnumObjects(HTREEITEM hParentItem, LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent);
	CEdit* m_pEditLabel;
	CString m_strCurrentPath;
	CString m_strCurrentLabel;
	bool canUpdateLabel;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
};


