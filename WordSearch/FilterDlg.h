#pragma once


// CFilterDlg 대화 상자

class CFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(CFilterDlg)

public:
	CFilterDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFilterDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlFilter;
	CRect m_rect;
	map<CString, bool>* m_mapFilter;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListFilter(NMHDR *pNMHDR, LRESULT *pResult);
};
