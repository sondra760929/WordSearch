// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// PDFIndexExplorerView.h: CPDFIndexExplorerView 클래스의 인터페이스
//

#pragma once
#include "MyCUGEdit.h"
#include <xlnt/xlnt.hpp>
#include "Ultimate_Grid\CellTypes\UGCTsarw.h"

class CPDFIndexExplorerDoc;
class CMainFrame;

class CPDFIndexExplorerView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CPDFIndexExplorerView();
	DECLARE_DYNCREATE(CPDFIndexExplorerView)

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PDFINDEXEXPLORER_FORM };
#endif

	// 특성입니다.
public:
	CPDFIndexExplorerDoc * GetDocument() const;

	// 작업입니다.
public:
	MyCugEdit m_Grid;
	CMainFrame* m_pFrame;
	CUGSortArrowType m_ArrowType;
	int m_iArrowIndex;

	vector<vector< pair<CString, CString>>>m_lstFindList;
	vector< CString > m_lstHeaderList;
	vector< vector< CString > > m_lstResultList;
	//vector< map< CString, bool>> m_lstFilter;
	//vector<vector<CString>> m_lstGridText;
	//vector< int > m_lstGridIndex;
	int m_iGridRowCount;
	bool is_fitGrid;
	FILE* log_fp;

	CString GetPathFromGridList(int row);
	void OnTH_LClicked(MyCugEdit* grid, int iCol, long lRow, int iUpdn);
	void OnTH_RClicked(MyCugEdit* grid, int iCol, long lRow, int iUpdn);
	void OnLClicked(MyCugEdit* grid, int col, long row, int updn);
	void CellModified(MyCugEdit* grid, int col, long row, LPCTSTR string);
	void DeleteRow(MyCugEdit* grid, CString sw_name);
	void ResizeControl(int cx, int cy);
	void SelectFileFromTree(CString file_path);
	void SelectResultFromTree(CString file_path, int page);
	void SelectResultFromTree(HTREEITEM parent_item, HTREEITEM tree_item);
	void FolderSearchFileName(CString folder_path, CString search_string);
	void FolderSearchPDF(CString folder_path, CString search_string, bool check_word, bool check_upperlower, bool check_bookmark, bool check_annotation);
	void TxtSearchPDF(CString file_path, CString search_txt, bool check_word, bool check_upperlower, bool check_bookmark, bool check_annotation);
	void AddOutput(CString file_path, CString search_para, int page, int index);
	void ResetOutput();
	void OnCellChange(int oldcol, int newcol, long oldrow, long newrow);
	void UpdateDataFromGrid();
	CString m_strCurrentSelectedItemPath;
	CString m_strCurrentExcelPath;
	int m_iCurrentSelectedItemType;
	void GetTotalExcelFile(CString folder_path, vector< CString >& total_file_list, vector< CString >& excel_file_list);
	//bool is_total_mode;
	CString m_strCurrentFolderPath;
	void FitGrid();
	bool CheckEncrypted(CString file_path);
	void SetEncrypted(CString file_path, CString password);
	void FolderGetPDFFiles(CString folder_path, vector< CString >& file_list);
	void OnDClicked(int col, long row);
	void FolderWordSearchPDF(CString folder_path);
	void TxtWordSearchTXT(CString file_path);
	void TxtWordSearchPDF(CString file_path);
	// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

	// 구현입니다.
public:
	virtual ~CPDFIndexExplorerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSearchList();
	afx_msg void OnUpdateSearchList(CCmdUI *pCmdUI);
	afx_msg void OnSearchFolder();
	afx_msg void OnUpdateSearchFolder(CCmdUI *pCmdUI);
	afx_msg void OnSearchFilename();
	afx_msg void OnUpdateSearchFilename(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSearchText(CCmdUI *pCmdUI);
	afx_msg void OnSearchText();
	//afx_msg void OnCheckFilepath();
	afx_msg void OnSaveFile();
	afx_msg void OnAddRow();
	afx_msg void OnDeleteRow();
	afx_msg void OnOpenSingle();
	afx_msg void OnOpenMerge();
	afx_msg void OnUpdateOpenMerge(CCmdUI *pCmdUI);
	afx_msg LRESULT OnClickGridHeader(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCheckWord();
	afx_msg void OnCheckUpperlower();
	afx_msg void OnCheckBookmark();
	afx_msg void OnCheckAnnotation();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnFileView();
	afx_msg void OnUpdateFileView(CCmdUI *pCmdUI);
	afx_msg void OnOutputWnd();
	afx_msg void OnUpdateOutputWnd(CCmdUI *pCmdUI);
	afx_msg void OnPropertiesWnd();
	afx_msg void OnUpdatePropertiesWnd(CCmdUI *pCmdUI);
	afx_msg void OnSetPassword();
	afx_msg void OnUnsetPassword();
};

#ifndef _DEBUG  // PDFIndexExplorerView.cpp의 디버그 버전
inline CPDFIndexExplorerDoc* CPDFIndexExplorerView::GetDocument() const
{
	return reinterpret_cast<CPDFIndexExplorerDoc*>(m_pDocument);
}
#endif

