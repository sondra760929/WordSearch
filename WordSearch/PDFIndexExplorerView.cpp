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

// PDFIndexExplorerView.cpp: CPDFIndexExplorerView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "PDFIndexExplorer.h"
#endif

#include "PDFIndexExplorerDoc.h"
#include "PDFIndexExplorerView.h"
#include "MainFrm.h"
#include "FilterDlg.h"
#include "resource.h"
#include "MFCRibbonCheckBox2.h"
#include "CInputDlg.h"
extern "C" {
#include <mupdf/fitz.h>
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
BOOL WriteStringValueInRegistry(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueKey, LPCTSTR lpValue)
{
	CString strValue = _T("");
	HKEY hSubKey = NULL;

	// open the key 
	if (::RegOpenKeyEx(hKey, lpSubKey, 0, KEY_ALL_ACCESS, &hSubKey) == ERROR_SUCCESS)
	{
		DWORD cbSize = (DWORD)strlen(lpValue) + 1;
		BYTE *pBuf = new BYTE[cbSize];
		::ZeroMemory(pBuf, cbSize);
		::CopyMemory(pBuf, lpValue, cbSize - 1);
		::RegSetValueEx(hSubKey, lpValueKey, NULL, REG_SZ, pBuf, cbSize);

		// 키 닫기
		::RegCloseKey(hSubKey);
		delete[] pBuf;
		return TRUE;
	}
	else
	{
		long status = ::RegCreateKeyEx(hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, NULL);
		if (status == ERROR_SUCCESS)
		{
			DWORD cbSize = (DWORD)strlen(lpValue) + 1;
			BYTE *pBuf = new BYTE[cbSize];
			::ZeroMemory(pBuf, cbSize);
			::CopyMemory(pBuf, lpValue, cbSize - 1);
			::RegSetValueEx(hSubKey, lpValueKey, NULL, REG_SZ, pBuf, cbSize);

			// 키 닫기
			::RegCloseKey(hSubKey);
			delete[] pBuf;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL WriteDWORDValueInRegistry(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueKey, DWORD dValue)
{
	CString strValue = _T("");
	HKEY hSubKey = NULL;

	// open the key 
	if (::RegOpenKeyEx(hKey, lpSubKey, 0, KEY_ALL_ACCESS, &hSubKey) == ERROR_SUCCESS)
	{
		::RegSetValueEx(hSubKey, lpValueKey, NULL, REG_DWORD, (const BYTE*)&dValue, sizeof(dValue));

		// 키 닫기
		::RegCloseKey(hSubKey);
		return TRUE;
	}
	else
	{
		long status = ::RegCreateKeyEx(hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, NULL);
		if (status == ERROR_SUCCESS)
		{
			::RegSetValueEx(hSubKey, lpValueKey, NULL, REG_DWORD, (const BYTE*)&dValue, sizeof(dValue));

			// 키 닫기
			::RegCloseKey(hSubKey);
			return TRUE;
		}
	}
	return FALSE;
}

CString ReadStringValueInRegistry(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueKey)
{
	CString strValue = _T("");
	HKEY hSubKey = NULL;

	// open the key
	if (::RegOpenKeyEx(hKey, lpSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
	{
		DWORD buf_size = 0;

		// 문자열의 크기를 먼저 읽어온다. 
		if (::RegQueryValueEx(hSubKey, lpValueKey, NULL, NULL, NULL, &buf_size) == ERROR_SUCCESS)
		{
			// 메모리 할당하고..., 
			TCHAR *pBuf = new TCHAR[buf_size + 1];

			// 실제 값을 읽어온다. 
			if (::RegQueryValueEx(hSubKey, lpValueKey, NULL, NULL, (LPBYTE)pBuf, &buf_size) == ERROR_SUCCESS)
			{
				pBuf[buf_size] = _T('\0');
				strValue = CString(pBuf);
			}

			// to avoid leakage 
			delete[] pBuf;
		}

		// 키 닫기 
		::RegCloseKey(hSubKey);
	}
	return strValue;
}

DWORD ReadDWORDValueInRegistry(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueKey)
{
	DWORD dwReturn = -1;
	DWORD dwBufSize = sizeof(DWORD);
	HKEY hSubKey = NULL;

	// open the key
	if (::RegOpenKeyEx(hKey, lpSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
	{
		DWORD error = RegQueryValueEx(hSubKey, lpValueKey, NULL, NULL, reinterpret_cast<LPBYTE>(&dwReturn), &dwBufSize);
		if (error == ERROR_SUCCESS)
		{
			printf("Key value is: %d \n", dwReturn);
		}
		else
		{
			printf("Cannot query for key value; Error is: %d\n", error);
		}
		// 키 닫기 
		::RegCloseKey(hSubKey);
	}
	return dwReturn;
}
void char_to_utf8(char* strMultibyte, char* out)
{
	memset(out, 0, 512);
	wchar_t strUni[512] = { 0, };
	int nLen = MultiByteToWideChar(CP_ACP, 0, strMultibyte, strlen(strMultibyte), NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, strMultibyte, strlen(strMultibyte), strUni, nLen);

	nLen = WideCharToMultiByte(CP_UTF8, 0, strUni, lstrlenW(strUni), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, strUni, lstrlenW(strUni), out, nLen, NULL, NULL);
}

void lpctstr_to_utf8(LPCTSTR in, char* out)
{
	char    strMultibyte[512] = { 0, };
	strcpy_s(strMultibyte, 512, in);
	char_to_utf8(strMultibyte, out);
}

string utf_to_multibyte(string in)
{
	wchar_t strUnicode[1024] = { 0, };
	char    strUTF8[1024] = { 0, };
	strcpy_s(strUTF8, 1024, in.c_str());// 이건 사실 멀티바이트지만 UTF8이라고 생각해주세요 -_-;;
	if (strlen(strUTF8) > 0)
	{
		int nLen = MultiByteToWideChar(CP_UTF8, 0, strUTF8, strlen(strUTF8), NULL, NULL);
		MultiByteToWideChar(CP_UTF8, 0, strUTF8, strlen(strUTF8), strUnicode, nLen);

		string strMulti = CW2A(strUnicode);
		return strMulti;
	}
	return "";
}


void SetCellValue(xlnt::worksheet& ws, const char* cell, CString value)
{
	if (value != "")
	{
		char strUtf8[512] = { 0, };

		lpctstr_to_utf8(value, strUtf8);
		ws.cell(cell).value(strUtf8);
	}
}

void SetCellValue(xlnt::worksheet& ws, int column, int row, CString value)
{
	if (value != "")
	{
		char strUtf8[512] = { 0, };

		lpctstr_to_utf8(value, strUtf8);
		ws.cell(column, row).value(strUtf8);
	}
}

CString GetFilePath(CString folder_path, CString sub_folder, CString filename)
{
	return folder_path + "\\" + sub_folder;// +"\\" + filename + ".pdf";
}

bool IsExistingFilePath(CString folder_path, CString sub_folder, CString filename)
{
	return PathFileExists(GetFilePath(folder_path, sub_folder, filename));
}

CString GetExistingFilePath(CString folder_path, CString sub_folder, CString filename)
{
	CString file_path = GetFilePath(folder_path, sub_folder, filename);
	if (PathFileExists(file_path))
	{
		return file_path;
	}
	return "";
}
// CPDFIndexExplorerView

IMPLEMENT_DYNCREATE(CPDFIndexExplorerView, CFormView)

BEGIN_MESSAGE_MAP(CPDFIndexExplorerView, CFormView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPDFIndexExplorerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_COMMAND(ID_SEARCH_LIST, &CPDFIndexExplorerView::OnSearchList)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_LIST, &CPDFIndexExplorerView::OnUpdateSearchList)
	ON_COMMAND(ID_SEARCH_FOLDER, &CPDFIndexExplorerView::OnSearchFolder)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FOLDER, &CPDFIndexExplorerView::OnUpdateSearchFolder)
	ON_COMMAND(ID_SEARCH_FILENAME, &CPDFIndexExplorerView::OnSearchFilename)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FILENAME, &CPDFIndexExplorerView::OnUpdateSearchFilename)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_TEXT, &CPDFIndexExplorerView::OnUpdateSearchText)
	ON_COMMAND(ID_SEARCH_TEXT, &CPDFIndexExplorerView::OnSearchText)
	//ON_COMMAND(ID_CHECK_FILEPATH, &CPDFIndexExplorerView::OnCheckFilepath)
	ON_COMMAND(ID_SAVE_FILE, &CPDFIndexExplorerView::OnSaveFile)
	ON_COMMAND(ID_ADD_ROW, &CPDFIndexExplorerView::OnAddRow)
	ON_COMMAND(ID_DELETE_ROW, &CPDFIndexExplorerView::OnDeleteRow)
	ON_COMMAND(ID_OPEN_SINGLE, &CPDFIndexExplorerView::OnOpenSingle)
	//ON_COMMAND(ID_OPEN_MERGE, &CPDFIndexExplorerView::OnOpenMerge)
	//ON_UPDATE_COMMAND_UI(ID_OPEN_MERGE, &CPDFIndexExplorerView::OnUpdateOpenMerge)
	ON_MESSAGE(WM_GRID_HEADER_CLILCK, &CPDFIndexExplorerView::OnClickGridHeader)
	ON_COMMAND(ID_CHECK_WORD, &CPDFIndexExplorerView::OnCheckWord)
	ON_COMMAND(ID_CHECK_UPPERLOWER, &CPDFIndexExplorerView::OnCheckUpperlower)
	ON_COMMAND(ID_CHECK_BOOKMARK, &CPDFIndexExplorerView::OnCheckBookmark)
	ON_COMMAND(ID_CHECK_ANNOTATION, &CPDFIndexExplorerView::OnCheckAnnotation)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_COMMAND(IDS_FILE_VIEW, &CPDFIndexExplorerView::OnFileView)
	ON_UPDATE_COMMAND_UI(IDS_FILE_VIEW, &CPDFIndexExplorerView::OnUpdateFileView)
	ON_COMMAND(IDS_OUTPUT_WND, &CPDFIndexExplorerView::OnOutputWnd)
	ON_UPDATE_COMMAND_UI(IDS_OUTPUT_WND, &CPDFIndexExplorerView::OnUpdateOutputWnd)
	ON_COMMAND(IDS_PROPERTIES_WND, &CPDFIndexExplorerView::OnPropertiesWnd)
	ON_UPDATE_COMMAND_UI(IDS_PROPERTIES_WND, &CPDFIndexExplorerView::OnUpdatePropertiesWnd)
	ON_COMMAND(ID_SET_PASSWORD, &CPDFIndexExplorerView::OnSetPassword)
	ON_COMMAND(ID_UNSET_PASSWORD, &CPDFIndexExplorerView::OnUnsetPassword)
END_MESSAGE_MAP()

// CPDFIndexExplorerView 생성/소멸

CPDFIndexExplorerView::CPDFIndexExplorerView()
	: CFormView(IDD_PDFINDEXEXPLORER_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_iGridRowCount = 0;
	is_fitGrid = false;
}

CPDFIndexExplorerView::~CPDFIndexExplorerView()
{
}

void CPDFIndexExplorerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CPDFIndexExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CPDFIndexExplorerView::OnInitialUpdate()
{
	CString current_path = ReadStringValueInRegistry(HKEY_CURRENT_USER, (LPCTSTR)m_strKey, _T("CURRENT_PATH"));

	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	//ResizeParentToFit();
	m_Grid.AttachGrid(this, IDC_GRID);
	//m_iArrowIndex = m_Grid.AddCellType(&m_sortArrow);
	m_Grid.m_pParentView = this;

	m_iArrowIndex = m_Grid.AddCellType(&m_ArrowType);

	m_Grid.SetVScrollMode(UG_SCROLLTRACKING);
	m_Grid.SetHScrollMode(UG_SCROLLTRACKING);
	m_Grid.SetSH_ColWidth(-1, 0);
	m_Grid.SetMargin(10);

	m_Grid.SetNumberCols(9);
	m_Grid.SetNumberRows(0);

	m_Grid.SetMultiSelectMode(TRUE);
	m_Grid.SetHighlightRow(TRUE);

	CRect rect;
	GetClientRect(&rect);
	ResizeControl(rect.Width(), rect.Height());

	m_pFrame = (CMainFrame*)AfxGetMainWnd();

	if (PathFileExists(current_path))
	{
		m_pFrame->m_wndFileView.SetFolder(current_path);
	}
	//SetTimer(1, 10, NULL);
}


// CPDFIndexExplorerView 인쇄


void CPDFIndexExplorerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPDFIndexExplorerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CPDFIndexExplorerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CPDFIndexExplorerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CPDFIndexExplorerView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}

void CPDFIndexExplorerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPDFIndexExplorerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CPDFIndexExplorerView 진단

#ifdef _DEBUG
void CPDFIndexExplorerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPDFIndexExplorerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPDFIndexExplorerDoc* CPDFIndexExplorerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPDFIndexExplorerDoc)));
	return (CPDFIndexExplorerDoc*)m_pDocument;
}
#endif //_DEBUG


// CPDFIndexExplorerView 메시지 처리기

CString CPDFIndexExplorerView::GetPathFromGridList(int row)
{
	int col_count = m_Grid.GetNumberCols();
	auto index_itr = find(m_lstGridIndex.begin(), m_lstGridIndex.end(), row);
	int data_index = index_itr - m_lstGridIndex.begin();

	if(data_index > -1 && data_index < m_lstGridIndex.size())
	{
		return m_lstGridText[data_index][col_count];
	}
	return "";
}

void CPDFIndexExplorerView::OnSearchList()
{
	int row_count = m_Grid.GetNumberRows();
	int col_count = m_Grid.GetNumberCols();

	if (row_count < 2)
	{
		AfxMessageBox("엑셀 파일을 선택하세요.");
		return;
	}

	CString search_string = m_pFrame->GetSearchText();
	search_string.Trim();
	if (search_string.GetLength() < 2)
	{
		AfxMessageBox("2개 이상의 글자를 입력하세요.");
		return;
	}

	vector< CString > file_list;
	for (int i = 0; i < row_count; i++)
	{

		CString file_path = GetFilePath(m_strCurrentFolderPath, GetPathFromGridList(i), m_Grid.QuickGetText(3, i));
		if (PathFileExists(file_path))
		{
			file_list.push_back(file_path);
		}
	}

	if (file_list.size() > 0)
	{
		CString temp_string;
		temp_string.Format("[%d]개의 PDF 파일을 대상으로 [%s]를 내용 검색하시겠습니까?", file_list.size(), search_string);
		if (AfxMessageBox(temp_string, MB_YESNO) == IDYES)
		{
			ResetOutput();
			for (int i = 0; i < file_list.size(); i++)
			{
				TxtSearchPDF(file_list[i], search_string, m_pFrame->GetCheckWord(), m_pFrame->GetCheckUpperLower(), m_pFrame->GetCheckBookMark(), m_pFrame->GetCheckAnnotation());
			}
			m_pFrame->UpdateOutputTitle();
		}
	}
}


void CPDFIndexExplorerView::OnUpdateSearchList(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CPDFIndexExplorerView::OnSearchFolder()
{
	CString temp_string;
	DWORD ret = GetFileAttributes(m_strCurrentSelectedItemPath);
	bool is_folder = false;
	if (ret != INVALID_FILE_ATTRIBUTES)
	{
		if (ret & FILE_ATTRIBUTE_DIRECTORY)
		{
			//	폴더임
			is_folder = true;
		}
	}

	CString folder_path = m_strCurrentSelectedItemPath;
	if (!is_folder)
	{
		int index = m_strCurrentSelectedItemPath.ReverseFind('\\');
		if (index > 0)
		{
			folder_path = m_strCurrentSelectedItemPath.Left(index);
		}
	}

	temp_string.Format("[%s] 폴더 하위의 모든 PDF 파일을 대상으로 단어 검색을 수행 하시겠습니까?", folder_path);
	if (AfxMessageBox(temp_string, MB_YESNO) == IDYES)
	{
		ResetOutput();
		FolderWordSearchPDF(folder_path);
		m_pFrame->m_wndFileView.SetFolder(folder_path);
		m_pFrame->UpdateOutputTitle();
	}
	
	//CString search_string = m_pFrame->GetSearchText();
	//search_string.Trim();
	//if (search_string.GetLength() < 2)
	//{
	//	AfxMessageBox("2개 이상의 글자를 입력하세요.");
	//	return;
	//}


	//switch (m_iCurrentSelectedItemType)
	//{
	//case 1:
	//{
	//	int index = m_strCurrentSelectedItemPath.ReverseFind('\\');
	//	if (index > 0)
	//	{
	//		CString folder_path = m_strCurrentSelectedItemPath.Left(index);
	//		temp_string.Format("[%s] 폴더 하위의 모든 PDF 파일을 대상으로 단어 검색을 수행 하시겠습니까?", folder_path);
	//		if (AfxMessageBox(temp_string, MB_YESNO) == IDYES)
	//		{
	//			ResetOutput();
	//			FolderSearchPDF(folder_path, search_string, m_pFrame->GetCheckWord(), m_pFrame->GetCheckUpperLower(), m_pFrame->GetCheckBookMark(), m_pFrame->GetCheckAnnotation());
	//			m_pFrame->UpdateOutputTitle();
	//		}
	//	}
	//}
	//break;
	//case 2:
	//{
	//	temp_string.Format("[%s] 파일을 대상으로 [%s]를 내용 검색하시겠습니까?", m_strCurrentSelectedItemPath, search_string);
	//	if (AfxMessageBox(temp_string, MB_YESNO) == IDYES)
	//	{
	//		ResetOutput();
	//		TxtSearchPDF(m_strCurrentSelectedItemPath, search_string, m_pFrame->GetCheckWord(), m_pFrame->GetCheckUpperLower(), m_pFrame->GetCheckBookMark(), m_pFrame->GetCheckAnnotation());
	//		m_pFrame->UpdateOutputTitle();
	//	}
	//}
	//break;
	//case 3:
	//{
	//	temp_string.Format("[%s] 폴더 하위의 모든 PDF 파일을 대상으로 단어 검색을 수행 하시겠습니까?", m_strCurrentSelectedItemPath);
	//	if (AfxMessageBox(temp_string, MB_YESNO) == IDYES)
	//	{
	//		ResetOutput();
	//		FolderSearchPDF(m_strCurrentSelectedItemPath, search_string, m_pFrame->GetCheckWord(), m_pFrame->GetCheckUpperLower(), m_pFrame->GetCheckBookMark(), m_pFrame->GetCheckAnnotation());
	//		m_pFrame->UpdateOutputTitle();
	//	}
	//}
	//break;
	//}
}


void CPDFIndexExplorerView::OnUpdateSearchFolder(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CPDFIndexExplorerView::OnSearchFilename()
{
	CString search_string = m_pFrame->GetSearchText();
	search_string.Trim();
	if (search_string.GetLength() < 2)
	{
		AfxMessageBox("2개 이상의 글자를 입력하세요.");
		return;
	}

	CString temp_string;

	switch (m_iCurrentSelectedItemType)
	{
	case 1:
	case 2:
	{
		int index = m_strCurrentSelectedItemPath.ReverseFind('\\');
		if (index > 0)
		{
			CString folder_path = m_strCurrentSelectedItemPath.Left(index);
			temp_string.Format("[%s] 폴더 하위의 모든 파일을 대상으로 [%s]를 파일명 검색하시겠습니까?", folder_path, search_string);
			if (AfxMessageBox(temp_string, MB_YESNO) == IDYES)
			{
				ResetOutput();
				FolderSearchFileName(folder_path, search_string);
				m_pFrame->UpdateOutputTitle();
			}
		}
	}
	break;
	case 3:
	{
		temp_string.Format("[%s] 폴더 하위의 모든 파일을 대상으로 [%s]를 파일명 검색하시겠습니까?", m_strCurrentSelectedItemPath, search_string);
		if (AfxMessageBox(temp_string, MB_YESNO) == IDYES)
		{
			ResetOutput();
			FolderSearchFileName(m_strCurrentSelectedItemPath, search_string);
			m_pFrame->UpdateOutputTitle();
		}
	}
	break;
	}
}


void CPDFIndexExplorerView::OnUpdateSearchFilename(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPDFIndexExplorerView::UpdateDataFromGrid()
{
	m_iGridRowCount = m_lstGridText.size();
	int col_size = m_Grid.GetNumberCols();

	//	화면에 표시된 내용으로 업데이트
	for (int i = 0; i < m_iGridRowCount; i++)
	{
		if (m_lstGridIndex[i] > -1)
		{
			for (int j = 0; j < col_size; j++)
			{
				m_lstGridText[i][j] = m_Grid.QuickGetText(j, m_lstGridIndex[i]);
			}
		}
	}
}

LRESULT CPDFIndexExplorerView::OnClickGridHeader(WPARAM wParam, LPARAM lParam)
{
	//int iCol = (int)wParam;
	//long lRow = (long)lParam;

	//if (iCol > 0 && lRow > -2)
	//{
	//	m_iGridRowCount = m_lstGridText.size();
	//	int col_size = m_Grid.GetNumberCols();
	//	
	//	//	화면에 표시된 내용으로 업데이트
	//	UpdateDataFromGrid();

	//	for (int i = 1; i < col_size-1; i++)
	//	{
	//		map< CString, bool > new_filter;
	//		for (int j = 0; j < m_iGridRowCount; j++)
	//		{
	//			if (new_filter.find(m_lstGridText[j][i]) == new_filter.end())
	//			{
	//				if (m_lstFilter[i].find(m_lstGridText[j][i]) == m_lstFilter[i].end())
	//				{
	//					new_filter[m_lstGridText[j][i]] = true;
	//				}
	//				else
	//				{
	//					new_filter[m_lstGridText[j][i]] = m_lstFilter[i][m_lstGridText[j][i]];
	//				}
	//			}
	//		}				
	//		m_lstFilter[i] = new_filter;
	//	}
	//	
	//	CRect cell_rect;
	//	CRect grid_rect;
	//	m_Grid.GetWindowRect(&grid_rect);
	//	m_Grid.GetCellRect(&iCol, &lRow, &cell_rect);
	//	CFilterDlg pdlg;
	//	pdlg.m_rect.SetRect(grid_rect.left + cell_rect.left, grid_rect.top + cell_rect.bottom, grid_rect.left + cell_rect.right, grid_rect.top + cell_rect.bottom);
	//	pdlg.m_mapFilter = &(m_lstFilter[iCol]);

	//	if (pdlg.DoModal() == IDOK)
	//	{
	//	}

	//	//	header 표시
	//	bool is_all = true;
	//	auto check = m_lstFilter[iCol].begin();
	//	while (check != m_lstFilter[iCol].end())
	//	{
	//		if (check->second == false)
	//			is_all = false;
	//		check++;
	//	}

	//	if (is_all)
	//	{
	//		m_Grid.QuickSetCellType(iCol, -1, 0);
	//	}
	//	else
	//	{
	//		m_Grid.QuickSetCellType(iCol, -1, m_iArrowIndex);
	//		m_Grid.QuickSetCellTypeEx(iCol, -1, UGCT_SORTARROWDOWN);
	//	}

	//	m_Grid.SetPaintMode(FALSE);

	//	int row_size = 0;
	//	m_Grid.SetNumberRows(0);
	//	for (int i = 0; i < m_iGridRowCount; i++)
	//	{
	//		bool is_show = true;
	//		for (int j = 1; j < col_size; j++)
	//		{
	//			if (m_lstFilter[j][m_lstGridText[i][j]] == false)
	//			{
	//				is_show = false;
	//				break;
	//			}
	//		}

	//		if (is_show)
	//		{
	//			m_Grid.SetNumberRows(row_size + 1);
	//			m_lstGridIndex[i] = row_size;

	//			for (int j = 0; j < col_size; j++)
	//			{
	//				m_Grid.QuickSetText(j, row_size, m_lstGridText[i][j]);
	//			}
	//			row_size++;
	//		}
	//		else
	//		{
	//			m_lstGridIndex[i] = -1;
	//		}
	//	}
	//	m_Grid.SetPaintMode(TRUE);
	//	//FitGrid();
	//}

	return 0;
}

void CPDFIndexExplorerView::OnTH_LClicked(MyCugEdit* grid, int iCol, long lRow, int iUpdn)
{
	//if (iUpdn == TRUE)
	//{
	//	if (iCol > 0 && lRow > -2 && iCol < m_Grid.GetNumberCols() -1)
	//	{
	//		CRect cell_rect;
	//		CRect grid_rect;
	//		m_Grid.GetWindowRect(&grid_rect);
	//		m_Grid.GetCellRect(&iCol, &lRow, &cell_rect);
	//		CFilterDlg pdlg;
	//		pdlg.m_rect.SetRect(grid_rect.left + cell_rect.left, grid_rect.top + cell_rect.top, grid_rect.left + cell_rect.right, grid_rect.top + cell_rect.bottom);
	//		if (pdlg.DoModal() == IDOK)
	//		{
	//			AfxMessageBox("Test");
	//		}
	//	}
	//}


	



	//int grid_index = -1;
	//if ((grid == &(m_Grid1[0])) || (grid == &(m_Grid1[1])) || (grid == &(m_Grid1[2])))
	//{
	//	grid_index = 0;
	//	m_Grid1[0].QuickSetCellType(m_iSortCol[grid_index], -1, 0);
	//	m_Grid1[1].QuickSetCellType(m_iSortCol[grid_index], -1, 0);
	//	m_Grid1[2].QuickSetCellType(m_iSortCol[grid_index], -1, 0);
	//}
	//else if (grid == &m_Grid2)
	//{
	//	grid_index = 1;
	//	grid->QuickSetCellType(m_iSortCol[grid_index], -1, 0);
	//}

	//if (grid_index == -1)
	//	return;



	//if (iCol == m_iSortCol[grid_index])
	//{
	//	if (m_bSortedAscending[grid_index])
	//		m_bSortedAscending[grid_index] = false;
	//	else
	//		m_bSortedAscending[grid_index] = true;
	//}
	//else
	//{
	//	m_iSortCol[grid_index] = iCol;
	//	m_bSortedAscending[grid_index] = true;
	//}

	//if (m_bSortedAscending[grid_index])
	//{
	//	if (grid_index == 0)
	//	{
	//		for (int g_index = 0; g_index < 3; g_index++)
	//		{
	//			m_Grid1[g_index].SortBy(iCol, UG_SORT_ASCENDING);
	//			m_Grid1[g_index].QuickSetCellType(m_iSortCol[grid_index], -1, m_iArrowIndex[grid_index]);
	//			m_Grid1[g_index].QuickSetCellTypeEx(m_iSortCol[grid_index], -1, UGCT_SORTARROWDOWN);
	//		}
	//	}
	//	else
	//	{
	//		grid->SortBy(iCol, UG_SORT_ASCENDING);
	//		grid->QuickSetCellType(m_iSortCol[grid_index], -1, m_iArrowIndex[grid_index]);
	//		grid->QuickSetCellTypeEx(m_iSortCol[grid_index], -1, UGCT_SORTARROWDOWN);
	//		//		((CFindAndSortDemo*)GetParent())->Trace( _T( "Sorted column %d ascending" ), iCol );
	//	}
	//}
	//else
	//{
	//	if (grid_index == 0)
	//	{
	//		for (int g_index = 0; g_index < 3; g_index++)
	//		{
	//			m_Grid1[g_index].SortBy(iCol, UG_SORT_DESCENDING);
	//			m_Grid1[g_index].QuickSetCellType(m_iSortCol[grid_index], -1, m_iArrowIndex[grid_index]);
	//			m_Grid1[g_index].QuickSetCellTypeEx(m_iSortCol[grid_index], -1, UGCT_SORTARROWUP);
	//		}
	//	}
	//	else
	//	{
	//		grid->SortBy(iCol, UG_SORT_DESCENDING);
	//		grid->QuickSetCellType(m_iSortCol[grid_index], -1, m_iArrowIndex[grid_index]);
	//		grid->QuickSetCellTypeEx(m_iSortCol[grid_index], -1, UGCT_SORTARROWUP);
	//		//		Trace( _T( "Sorted column %d descending" ), iCol );
	//	}
	//}

	//grid->RedrawAll();
}

void CPDFIndexExplorerView::OnTH_RClicked(MyCugEdit* grid, int iCol, long lRow, int iUpdn)
{
	//int grid_index = -1;
	//if (grid == &(m_Grid1[0]))
	//{
	//	if (iCol == 3 || iCol == 4)
	//	{
	//		CTextFilterDlg pDlg(iCol, this);
	//		if (pDlg.DoModal() == IDOK)
	//		{
	//			filter_index = iCol;
	//			filter_string = string(pDlg.m_strFilter);
	//			RefreshGrid();
	//		}
	//	}
	//}

	//grid->RedrawAll();
}

void CPDFIndexExplorerView::DeleteRow(MyCugEdit* grid, CString sw_name)
{
	//string str_sw_name(sw_name);
	//if (grid == &(m_Grid1[0]))
	//{
	//	if (sw_map.find(str_sw_name) != sw_map.end())
	//	{
	//		sw_map.erase(str_sw_name);
	//	}
	//}
	//else if (grid == &(m_Grid1[1]))
	//{
	//	if (find_map.find(str_sw_name) != find_map.end())
	//	{
	//		find_map.erase(str_sw_name);
	//	}
	//}
	//else if (grid == &(m_Grid1[2]))
	//{
	//	if (del_map.find(str_sw_name) != del_map.end())
	//	{
	//		del_map.erase(str_sw_name);
	//	}
	//}
}


void CPDFIndexExplorerView::OnLClicked(MyCugEdit* grid, int col, long row, int updn)
{
}

void CPDFIndexExplorerView::CellModified(MyCugEdit* grid, int col, long row, LPCTSTR string)
{
}



void CPDFIndexExplorerView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	ResizeControl(cx, cy);
}

void CPDFIndexExplorerView::ResizeControl(int cx, int cy)
{
	int offset = 10;

	m_Grid.MoveWindow(offset, offset, cx - (offset * 2), cy - (offset * 2));
}

void CPDFIndexExplorerView::GetTotalExcelFile(CString folder_path, vector< CString >& total_file_list, vector< CString >& excel_file_list)
{
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(folder_path);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it
		CString str = finder.GetFilePath();
		CString check_str = str;
		check_str.MakeLower();
		if (!finder.IsDirectory())
		{
			if (str.Right(4).MakeLower() == "xlsx")
			{
				CString file_name = finder.GetFileName();
				if (file_name.Find("통합") > -1)
				{
					total_file_list.push_back(str);
				}
				else
				{
					excel_file_list.push_back(str);
				}
			}
		}
	}
	finder.Close();
}

void CPDFIndexExplorerView::SelectFileFromTree(CString file_path)
{
	WriteStringValueInRegistry(HKEY_CURRENT_USER, (LPCTSTR)m_strKey, _T("CURRENT_PATH"), file_path);

	m_strCurrentSelectedItemPath = file_path;
	CString ext = file_path.Right(file_path.GetLength() - file_path.ReverseFind('.') - 1);
	
	if (ext == "pdf")
	{
		m_iCurrentSelectedItemType = 2;
		m_pFrame->m_wndProperties.DoPreview(file_path);
		//SetTimer(1, 10, NULL);
	}
	else if (ext == "xls" || ext == "xlsx")
	{
		OnOpenSingle();
	}
	else
	{
		m_iCurrentSelectedItemType = 3;
	}

	//while (!(m_pFrame->GetFocus()->m_hWnd == m_pFrame->m_wndFileView.m_hWnd || m_pFrame->GetFocus()->m_hWnd == m_pFrame->m_wndFileView.m_wndFileView.m_hWnd))
	//{
	//	m_pFrame->m_wndFileView.SetFocus();
	//}
}

void CPDFIndexExplorerView::SelectResultFromTree(HTREEITEM parent_item, HTREEITEM tree_item)
{
	int page = (int)(m_pFrame->m_wndOutput.m_wndOutputFind.GetItemData(tree_item));
	CString file_path = m_pFrame->m_htreeitem_to_fname[parent_item];
	SelectResultFromTree(file_path, page);
}

void CPDFIndexExplorerView::SelectResultFromTree(CString file_path, int page)
{
	m_strCurrentSelectedItemPath = file_path;
	m_iCurrentSelectedItemType = 2;
	m_pFrame->m_wndProperties.DoPreview(file_path, page);
	m_pFrame->m_wndOutput.m_wndOutputFind.SetFocus();
	//SetTimer(3, 10, NULL);
}

void CPDFIndexExplorerView::FolderSearchFileName(CString folder_path, CString search_string)
{
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(folder_path);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();


		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it
		CString str = finder.GetFilePath();
		CString check_str = str;
		check_str.MakeLower();
		if (finder.IsDirectory())
		{
			FolderSearchFileName(str, search_string);
		}
		else
		{
			if (str.Right(3).MakeLower() == "pdf")
			{
				CString file_name = finder.GetFileName();
				if (file_name.Find(search_string) > -1)
				{
					AddOutput(str, file_name, 1, 0);
				}
			}
		}
	}

	finder.Close();
}

void CPDFIndexExplorerView::FolderWordSearchPDF(CString folder_path)
{
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(folder_path);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();


		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it
		CString str = finder.GetFilePath();
		CString check_str = str;
		check_str.MakeLower();
		if (finder.IsDirectory())
		{
			FolderWordSearchPDF(str);
		}
		else
		{
			if (str.Right(3).MakeLower() == "pdf")
			{
				TxtWordSearchPDF(str);
			}
		}
	}

	finder.Close();
}

void CPDFIndexExplorerView::FolderSearchPDF(CString folder_path, CString search_string, bool check_word, bool check_upperlower, bool check_bookmark, bool check_annotation)
{
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(folder_path);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();


		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it
		CString str = finder.GetFilePath();
		CString check_str = str;
		check_str.MakeLower();
		if (finder.IsDirectory())
		{
			FolderSearchPDF(str, search_string, check_word, check_upperlower, check_bookmark, check_annotation);
		}
		else
		{
			if (str.Right(3).MakeLower() == "pdf")
			{
				TxtSearchPDF(str, search_string, check_word, check_upperlower, check_bookmark, check_annotation);
			}
		}
	}

	finder.Close();
}

void CPDFIndexExplorerView::FolderGetPDFFiles(CString folder_path, vector< CString >& file_list)
{
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(folder_path);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();


		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it
		CString str = finder.GetFilePath();
		CString check_str = str;
		check_str.MakeLower();
		if (finder.IsDirectory())
		{
			FolderGetPDFFiles(str, file_list);
		}
		else
		{
			if (str.Right(3).MakeLower() == "pdf")
			{
				file_list.push_back(str);
			}
		}
	}

	finder.Close();
}

void CPDFIndexExplorerView::TxtWordSearchPDF(CString file_path)
{
	if (file_path.IsEmpty())
		return;

#ifdef _DEBUG
	DWORD dwTick = GetTickCount();
	CString strLog;
#endif

	//FILE* fp;
	//fopen_s(&fp, file_path + ".txt", "wt");
	CString strFile;
	fz_context *ctx;
	fz_document* doc;

	fz_matrix ctm;
	fz_page *page;
	fz_device *dev;
	fz_text_page *text;
	fz_text_sheet *sheet;
	int page_i, line, rotation, pagecount;
	vector< CString > m_lstGridSearchWord;
	int search_rows = m_lstGridText.size();
	int search_cols = 0;
	if (search_rows > 0)
	{
		search_cols = m_lstGridText[0].size();
		m_lstGridSearchWord.resize(search_cols);
	}
	char strUtf8[512] = { 0, };
	lpctstr_to_utf8(file_path, strUtf8);
	std::string s(strUtf8);

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	fz_try(ctx) {
		fz_register_document_handler(ctx, &pdf_document_handler);
		doc = fz_open_document(ctx, strUtf8);
	}fz_catch(ctx) {
		fz_free_context(ctx);
		return;
	}

	line = 0;
	rotation = 0;
	pagecount = 0;
	pagecount = fz_count_pages(doc);
	bool find_first = false;
	vector< int > find_list;
	vector< vector< bool > > find_sub_list;
	CString prev_text;
	int find_word_index = -1;

	fz_rotate(&ctm, rotation);
	fz_pre_scale(&ctm, 1.0f, 1.0f);

	sheet = fz_new_text_sheet(ctx);
	for (page_i = 0; page_i < pagecount; page_i++) {
		page = fz_load_page(doc, page_i);
		text = fz_new_text_page(ctx);
		dev = fz_new_text_device(ctx, sheet, text);

		//fz_enable_device_hints(dev, FZ_IGNORE_SHADE | FZ_NO_CACHE | FZ_DONT_INTERPOLATE_IMAGES);

#ifdef _DEBUG
		dwTick = GetTickCount();
#endif
		fz_run_page(doc, page, dev, &ctm, NULL);

#ifdef _DEBUG
		strLog.Format("run page:%d ms\n", GetTickCount() - dwTick);
		OutputDebugString(strLog);
		dwTick = GetTickCount();
#endif

		//m_linesInfoVector.push_back(line);
		//print_text_page(ctx, m_strContent, text, line);
		int block_n;
		char dst[1024];

		//for (block_n = 0; block_n < text->len; block_n++)
		//{
		//	if (text->blocks[block_n].type == FZ_PAGE_BLOCK_TEXT)
		//	{
		//		fz_text_block *block = text->blocks[block_n].u.text;
		//		fz_text_line *line;
		//		fz_text_char *ch;
		//		char utf[10];
		//		int i, n;

		//			for (line = block->lines; line < block->lines + block->len; line++)
		//			{
		//				fz_text_span *span;
		//				for (span = line->first_span; span; span = span->next)
		//				{
		//					for (ch = span->text; ch < span->text + span->len; ch++)
		//					{
		//						n = fz_runetochar(utf, ch->c);
		//						for (i = 0; i < n; i++)
		//							fprintf_s(fp, "%c", utf[i]);
		//					}
		//					/* SumatraPDF: separate spans with spaces */
		//					if (span->next && span->len > 0 && span->text[span->len - 1].c != ' ')
		//						fprintf_s(fp, " ");
		//				}
		//				//fprintf_s(fp, "\n");
		//			}
		//			fprintf_s(fp, "\n");
		//	}
		//}


		for (block_n = 0; block_n < text->len && find_word_index < 0; block_n++)
		{
			switch (text->blocks[block_n].type)
			{
			case FZ_PAGE_BLOCK_TEXT:
			{
				fz_text_block *block = text->blocks[block_n].u.text;
				fz_text_line *line;
				fz_text_char *ch;
				char utf[10];
				string output_string;
				int ch_i, n;
				string result_string;
				char result_chr[4096];
				char* result_chr_ptr = result_chr;
				int chr_count = 0;
				//for (line = block->lines; line < block->lines + block->len; line++)
				//{
				//	fz_text_span *span;
				//	for (span = line->first_span; span; span = span->next)
				//	{
				//		char* dstptr = dst;
				//		for (ch = span->text; ch < span->text + span->len; ch++)
				//		{
				//			dstptr += fz_runetochar(dstptr, ch->c);
				//			//for (ch_i = 0; ch_i < n; ch_i++)
				//			//{
				//			//	output_string += (utf[ch_i]);
				//			//}
				//			//fz_printf(out, "%c", utf[i]);
				//		}
				//		/* SumatraPDF: separate spans with spaces */
				//		if (span->next && span->len > 0 && span->text[span->len - 1].c != ' ')
				//		{
				//			dstptr[0] = ' ';
				//			dstptr++;
				//			//output_string += ' ';
				//			//output_string.AppendChar(' ');
				//		}
				//		//fz_printf(out, " ");
				//		*dstptr = '\0';
				//		result_string += utf_to_multibyte(string(dst));
				//	}
				//	//result_string += '\n';
				//}
				//CString str_result_string(result_string.c_str());
				CString str_result_string;
				CString str_temp_string;

				try
				{
					for (line = block->lines; line < block->lines + block->len; line++)
					{
						fz_text_span *span;
						for (span = line->first_span; span; span = span->next)
						{
							for (ch = span->text; ch < span->text + span->len; ch++)
							{
								n = fz_runetochar(utf, ch->c);
								for (ch_i = 0; ch_i < n; ch_i++)
								{
									sprintf_s(result_chr_ptr, 2048, "%c", utf[ch_i]);
									result_chr_ptr++;
									chr_count++;
								}
							}
							/* SumatraPDF: separate spans with spaces */
							if (span->next && span->len > 0 && span->text[span->len - 1].c != ' ')
							{
								sprintf_s(result_chr_ptr, 2048, " ");
								result_chr_ptr++;
								chr_count++;
							}
						}

						if (chr_count > 2048)
						{
							sprintf_s(result_chr_ptr, 2048, "\n");
							chr_count++;
							//fprintf_s(fp, "%s", result_chr);

							wchar_t* strUnicode = new wchar_t[chr_count + 1]{ 0, };
							int nLen = MultiByteToWideChar(CP_UTF8, 0, result_chr, chr_count, NULL, NULL);
							MultiByteToWideChar(CP_UTF8, 0, result_chr, chr_count, strUnicode, nLen);

							string strMulti = CW2A(strUnicode);
							delete strUnicode;

							str_temp_string.Format("%s", strMulti.c_str());
							str_result_string += str_temp_string;

							chr_count = 0;
							result_chr_ptr = result_chr;
						}
						//fprintf_s(fp, "\n");
					}
					sprintf_s(result_chr_ptr, 4096, "\n");
					chr_count++;
					//fprintf_s(fp, "%s", result_chr);

					wchar_t* strUnicode = new wchar_t[chr_count + 1]{ 0, };
					int nLen = MultiByteToWideChar(CP_UTF8, 0, result_chr, chr_count, NULL, NULL);
					MultiByteToWideChar(CP_UTF8, 0, result_chr, chr_count, strUnicode, nLen);

					string strMulti = CW2A(strUnicode);
					delete strUnicode;

					str_temp_string.Format("%s", strMulti.c_str());
					str_result_string += str_temp_string;

					//if (!check_upperlower)
					//{
					//	str_result_string = str_result_string.MakeLower();
					//}
					if (!find_first)
					{
						for (int ti = 0; ti < search_rows; ti++)
						{
							CString search_txt = m_lstGridText[ti][0];
							int find_index = str_result_string.Find(search_txt);
							if (find_index > -1)
							{
								find_list.push_back(ti);
								vector< bool > sub_list;
								sub_list.resize(search_cols, false);
								find_sub_list.push_back(sub_list);
								find_first = true;

								CString sub_string;
								sub_string.Format("Page(%d) : [%d] %s", page_i + 1, ti + 1, search_txt);
								AddOutput(file_path, sub_string, page_i + 1, find_index);
							}
						}
					}
					prev_text += str_result_string;

					if(find_first)
					{
						if (find_list.size() == 1)
						{
							//	검색 멈춤, 결과 선택
							find_word_index = find_list[0];
							break;
						}
						else
						{
							for (int tj = 1; tj < search_cols; tj++)
							{
								int find_count = 0;
								int find_row_index = -1;
								for (int ti = 0; ti < find_list.size(); ti++)
								{
									CString search_txt = m_lstGridText[find_list[ti]][tj];
									int find_index = prev_text.Find(search_txt);
									if (find_index > -1)
									{
										find_sub_list[ti][tj] = true;
										find_count++;
										find_row_index = find_list[ti];

										CString sub_string;
										sub_string.Format("Page(%d) : [%d] %s", page_i + 1, find_row_index + 1, search_txt);
										AddOutput(file_path, sub_string, page_i + 1, find_index);
									}
								}

								if (find_count == 1)
								{
									//	하나만 검색 됨. 검색 멈춤, 결과 선택
									find_word_index = find_row_index;
									break;
								}
							}
							prev_text = "";
						}
					}
				}
				catch (...)
				{
				}

				break;
			}
			case FZ_PAGE_BLOCK_IMAGE:
				fz_text_block *block = text->blocks[block_n].u.text;
				break;
			}
		}


#ifdef _DEBUG
		strLog.Format("print text:%d ms\n", GetTickCount() - dwTick);
		OutputDebugString(strLog);
		dwTick = GetTickCount();
#endif

		fz_free_device(dev);
		fz_free_text_page(ctx, text);
		fz_free_page(doc, page);
	}

	fz_free_text_sheet(ctx, sheet);
	fz_close_document(doc);
	fz_free_context(ctx);

	if (find_word_index > -1)
	{
		CString sub_string;
		int index2 = file_path.ReverseFind('\\');
		CString new_file_path;
		new_file_path = file_path.Left(index2) + "\\";
		for (int tj = 0; tj < search_cols; tj++)
		{
			if (tj > 0)
				new_file_path += "_";
			new_file_path += m_lstGridText[find_word_index][tj];
		}
		new_file_path += ".pdf";
		sub_string.Format("rename [%s] >> [%s]", file_path, new_file_path);
		AddOutput(file_path, sub_string, 0, 0);

		rename(file_path, new_file_path);
	}
	//fclose(fp);
}
void CPDFIndexExplorerView::TxtSearchPDF(CString file_path, CString search_txt, bool check_word, bool check_upperlower, bool check_bookmark, bool check_annotation)
{
	if (file_path.IsEmpty())
		return;

#ifdef _DEBUG
	DWORD dwTick = GetTickCount();
	CString strLog;
#endif

	if (check_word)
	{
		search_txt.Trim();
		search_txt = " " + search_txt + " ";
	}

	if (!check_upperlower)
	{
		search_txt = search_txt.MakeLower();
	}

	//FILE* fp;
	//fopen_s(&fp, file_path + ".txt", "wt");
	CString strFile;
	fz_context *ctx;
	fz_document* doc;

	fz_matrix ctm;
	fz_page *page;
	fz_device *dev;
	fz_text_page *text;
	fz_text_sheet *sheet;
	int page_i, line, rotation, pagecount;

	char strUtf8[512] = { 0, };
	lpctstr_to_utf8(file_path, strUtf8);
	std::string s(strUtf8);

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	fz_try(ctx) {
		fz_register_document_handler(ctx, &pdf_document_handler);
		doc = fz_open_document(ctx, strUtf8);
	}fz_catch(ctx) {
		fz_free_context(ctx);
		return;
	}

	line = 0;
	rotation = 0;
	pagecount = 0;
	pagecount = fz_count_pages(doc);

	fz_rotate(&ctm, rotation);
	fz_pre_scale(&ctm, 1.0f, 1.0f);

	sheet = fz_new_text_sheet(ctx);
	for (page_i = 0; page_i < pagecount; page_i++) {
		page = fz_load_page(doc, page_i);
		text = fz_new_text_page(ctx);
		dev = fz_new_text_device(ctx, sheet, text);

		//fz_enable_device_hints(dev, FZ_IGNORE_SHADE | FZ_NO_CACHE | FZ_DONT_INTERPOLATE_IMAGES);

#ifdef _DEBUG
		dwTick = GetTickCount();
#endif
		fz_run_page(doc, page, dev, &ctm, NULL);

#ifdef _DEBUG
		strLog.Format("run page:%d ms\n", GetTickCount() - dwTick);
		OutputDebugString(strLog);
		dwTick = GetTickCount();
#endif

		//m_linesInfoVector.push_back(line);
		//print_text_page(ctx, m_strContent, text, line);
		int block_n;
		char dst[1024];

		//for (block_n = 0; block_n < text->len; block_n++)
		//{
		//	if (text->blocks[block_n].type == FZ_PAGE_BLOCK_TEXT)
		//	{
		//		fz_text_block *block = text->blocks[block_n].u.text;
		//		fz_text_line *line;
		//		fz_text_char *ch;
		//		char utf[10];
		//		int i, n;

		//			for (line = block->lines; line < block->lines + block->len; line++)
		//			{
		//				fz_text_span *span;
		//				for (span = line->first_span; span; span = span->next)
		//				{
		//					for (ch = span->text; ch < span->text + span->len; ch++)
		//					{
		//						n = fz_runetochar(utf, ch->c);
		//						for (i = 0; i < n; i++)
		//							fprintf_s(fp, "%c", utf[i]);
		//					}
		//					/* SumatraPDF: separate spans with spaces */
		//					if (span->next && span->len > 0 && span->text[span->len - 1].c != ' ')
		//						fprintf_s(fp, " ");
		//				}
		//				//fprintf_s(fp, "\n");
		//			}
		//			fprintf_s(fp, "\n");
		//	}
		//}


		for (block_n = 0; block_n < text->len; block_n++)
		{
			switch (text->blocks[block_n].type)
			{
			case FZ_PAGE_BLOCK_TEXT:
			{
				fz_text_block *block = text->blocks[block_n].u.text;
				fz_text_line *line;
				fz_text_char *ch;
				char utf[10];
				string output_string;
				int ch_i, n;
				string result_string;
				char result_chr[4096];
				char* result_chr_ptr = result_chr;
				int chr_count = 0;
				//for (line = block->lines; line < block->lines + block->len; line++)
				//{
				//	fz_text_span *span;
				//	for (span = line->first_span; span; span = span->next)
				//	{
				//		char* dstptr = dst;
				//		for (ch = span->text; ch < span->text + span->len; ch++)
				//		{
				//			dstptr += fz_runetochar(dstptr, ch->c);
				//			//for (ch_i = 0; ch_i < n; ch_i++)
				//			//{
				//			//	output_string += (utf[ch_i]);
				//			//}
				//			//fz_printf(out, "%c", utf[i]);
				//		}
				//		/* SumatraPDF: separate spans with spaces */
				//		if (span->next && span->len > 0 && span->text[span->len - 1].c != ' ')
				//		{
				//			dstptr[0] = ' ';
				//			dstptr++;
				//			//output_string += ' ';
				//			//output_string.AppendChar(' ');
				//		}
				//		//fz_printf(out, " ");
				//		*dstptr = '\0';
				//		result_string += utf_to_multibyte(string(dst));
				//	}
				//	//result_string += '\n';
				//}
				//CString str_result_string(result_string.c_str());
				CString str_result_string;
				CString str_temp_string;

				try
				{
					for (line = block->lines; line < block->lines + block->len; line++)
					{
						fz_text_span *span;
						for (span = line->first_span; span; span = span->next)
						{
							for (ch = span->text; ch < span->text + span->len; ch++)
							{
								n = fz_runetochar(utf, ch->c);
								for (ch_i = 0; ch_i < n; ch_i++)
								{
									sprintf_s(result_chr_ptr, 2048, "%c", utf[ch_i]);
									result_chr_ptr++;
									chr_count++;
								}
							}
							/* SumatraPDF: separate spans with spaces */
							if (span->next && span->len > 0 && span->text[span->len - 1].c != ' ')
							{
								sprintf_s(result_chr_ptr, 2048, " ");
								result_chr_ptr++;
								chr_count++;
							}
						}

						if (chr_count > 2048)
						{
							sprintf_s(result_chr_ptr, 2048, "\n");
							chr_count++;
							//fprintf_s(fp, "%s", result_chr);

							wchar_t* strUnicode = new wchar_t[chr_count + 1]{ 0, };
							int nLen = MultiByteToWideChar(CP_UTF8, 0, result_chr, chr_count, NULL, NULL);
							MultiByteToWideChar(CP_UTF8, 0, result_chr, chr_count, strUnicode, nLen);

							string strMulti = CW2A(strUnicode);
							delete strUnicode;

							str_temp_string.Format("%s", strMulti.c_str());
							str_result_string += str_temp_string;

							chr_count = 0;
							result_chr_ptr = result_chr;
						}
						//fprintf_s(fp, "\n");
					}
					sprintf_s(result_chr_ptr, 4096, "\n");
					chr_count++;
					//fprintf_s(fp, "%s", result_chr);

					wchar_t* strUnicode = new wchar_t[chr_count + 1]{ 0, };
					int nLen = MultiByteToWideChar(CP_UTF8, 0, result_chr, chr_count, NULL, NULL);
					MultiByteToWideChar(CP_UTF8, 0, result_chr, chr_count, strUnicode, nLen);

					string strMulti = CW2A(strUnicode);
					delete strUnicode;

					str_temp_string.Format("%s", strMulti.c_str());
					str_result_string += str_temp_string;

					if (!check_upperlower)
					{
						str_result_string = str_result_string.MakeLower();
					}

					int find_index = str_result_string.Find(search_txt);
					while (find_index > -1)
					{
						int start_index = find_index;// -50;
						if (start_index < 0) start_index = 0;
						int sub_length = search_txt.GetLength() + 100;
						if (start_index + sub_length > str_result_string.GetLength())
						{
							sub_length = str_result_string.GetLength() - start_index;
						}
						else
						{
							int end_index = str_result_string.Find(" ", start_index + sub_length);
							if (end_index > -1)
							{
								sub_length = end_index - start_index;
							}
						}
						CString sub_string;
						sub_string.Format("Page(%d) : ", page_i + 1);
						sub_string += str_result_string.Mid(start_index, sub_length);
						//sub_string.Insert(find_index - start_index, "[");
						//sub_string.Insert(find_index - start_index + search_txt.GetLength() + 1, "]");
						AddOutput(file_path, sub_string, page_i + 1, find_index);
						find_index = str_result_string.Find(search_txt, find_index + 1);
					}
				}
				catch (...)
				{
				}

				break;
			}
			case FZ_PAGE_BLOCK_IMAGE:
				fz_text_block *block = text->blocks[block_n].u.text;
				break;
			}
		}


#ifdef _DEBUG
		strLog.Format("print text:%d ms\n", GetTickCount() - dwTick);
		OutputDebugString(strLog);
		dwTick = GetTickCount();
#endif

		fz_free_device(dev);
		fz_free_text_page(ctx, text);
		fz_free_page(doc, page);
	}

	fz_free_text_sheet(ctx, sheet);
	fz_close_document(doc);
	fz_free_context(ctx);

	//fclose(fp);
}

void CPDFIndexExplorerView::AddOutput(CString file_path, CString search_para, int page, int index)
{
	m_pFrame->AddOutput(file_path, search_para, page, index);
}

void CPDFIndexExplorerView::ResetOutput()
{
	m_pFrame->ResetOutput();
}


void CPDFIndexExplorerView::OnUpdateSearchText(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CPDFIndexExplorerView::OnSearchText()
{
	//OnSearchList();
}


//void CPDFIndexExplorerView::OnCheckFilepath()
//{
//	UpdateDataFromGrid();
//	m_iGridRowCount = m_lstGridText.size();
//	int col_count = m_Grid.GetNumberCols();
//
//	if (is_total_mode)
//	{
//		//int row_count = m_Grid.GetNumberRows();
//		for (int row = 0; row < m_iGridRowCount; row++)
//		{
//			CString existing_file_path = GetExistingFilePath(m_strCurrentFolderPath, m_lstGridText[row][2], m_lstGridText[row][1]);
//			if (existing_file_path != "")
//			{
//				m_lstGridText[row][col_count - 1] = existing_file_path;
//				if (m_lstGridIndex[row] > -1)
//				{
//					m_Grid.QuickSetText(col_count - 1, m_lstGridIndex[row], existing_file_path);
//				}
//			}
//		}
//
//		FitGrid();
//	}
//	else
//	{
//		CString file_name = m_strCurrentExcelPath.Right(m_strCurrentExcelPath.GetLength() - m_strCurrentExcelPath.ReverseFind('\\') - 1);
//		file_name = file_name.Left(file_name.GetLength() - 12);
//		for (int row = 0; row < m_iGridRowCount; row++)
//		{
//			CString existing_file_path = GetExistingFilePath(m_strCurrentFolderPath, file_name, m_lstGridText[row][1]);
//			if (existing_file_path != "")
//			{
//				m_lstGridText[row][col_count - 1] = existing_file_path;
//				if (m_lstGridIndex[row] > -1)
//				{
//					m_Grid.QuickSetText(col_count - 1, m_lstGridIndex[row], existing_file_path);
//				}
//			}
//		}
//		FitGrid();
//	}
//}

void CPDFIndexExplorerView::FitGrid()
{
	if (!is_fitGrid)
	{
		is_fitGrid = true;
		m_Grid.BestFit(0, m_Grid.GetNumberCols() - 1, 0, UG_BESTFIT_TOPHEADINGS);
	}
	m_Grid.RedrawAll();
}

void CPDFIndexExplorerView::OnSaveFile()
{
	int row_count = m_Grid.GetNumberRows();
	int col_count = m_Grid.GetNumberCols();
	if (row_count > 0)
	{
		CFileDialog dlg(FALSE, "xlsx", m_strCurrentExcelPath, 6UL, "Excel(*.xlsx)|*.xlsx|All Files(*.*)|*.*||");
		//if (is_total_mode)
		//{
		//	dlg.m_ofn.lpstrInitialDir = "전자파일 통합리스트.xlsx";
		//}
		//else
		//{
			dlg.m_ofn.lpstrInitialDir = m_strCurrentFolderPath;
		//}
		if (dlg.DoModal() == IDOK)
		{
			xlnt::workbook wb;
			xlnt::worksheet ws = wb.active_sheet();

			xlnt::border::border_property bp;
			bp.color(xlnt::rgb_color(0, 0, 0));
			bp.style(xlnt::border_style::thin);
			xlnt::border border;
			border.side(xlnt::border_side::bottom, bp)
				.side(xlnt::border_side::top, bp)
				.side(xlnt::border_side::start, bp)
				.side(xlnt::border_side::end, bp);

			xlnt::alignment alignment;
			alignment.horizontal(xlnt::horizontal_alignment::center);

			xlnt::font font;
			font.color(xlnt::rgb_color(255, 255, 255));

			//for (int i = -1; i < row_count; i++)
			//{
			//	for (int j = 3; j < col_count - 1; j++)
			//	{
			//		SetCellValue(ws, j - 2, i + 2, m_Grid.QuickGetText(j, i));
			//	}
			//	SetCellValue(ws, 26, i + 2, m_Grid.QuickGetText(col_count - 1, i));
			//}

			for (int j = 3; j < col_count; j++)
			{
				SetCellValue(ws, j - 2, 1, m_Grid.QuickGetText(j, -1));
			}
			SetCellValue(ws, 26, 1, "파일 경로");
			
			for (int i = 0; i < m_lstGridText.size(); i++)
			{
				for (int j = 3; j < col_count; j++)
				{
					SetCellValue(ws, j-2, i + 2, m_lstGridText[i][j]);
				}
				SetCellValue(ws, 26, i + 2, m_lstGridText[i][col_count]);
			}
			char    UTF[512] = { 0, };
			lpctstr_to_utf8(dlg.GetPathName(), UTF);
			wb.save(std::string(UTF));

			CString temp_string;
			temp_string.Format("[%s] 파일이 생성되었습니다.", dlg.GetPathName());
			AfxMessageBox(temp_string);
		}
	}
}


void CPDFIndexExplorerView::OnAddRow()
{
	int row_count = m_Grid.GetNumberRows();
	int data_count = m_lstGridText.size();
	m_Grid.SetNumberRows(row_count + 1);
	m_lstGridText.resize(data_count + 1);
	m_lstGridText[data_count].resize(m_Grid.GetNumberCols());
	m_lstGridIndex.resize(data_count + 1);
	m_lstGridIndex[data_count] = row_count;
	CString temp_string;
	temp_string.Format("%d", data_count + 1);
	m_lstGridText[data_count][0] = temp_string;
	m_Grid.QuickSetText(0, row_count, temp_string);
	m_Grid.RedrawAll();
}


void CPDFIndexExplorerView::OnDeleteRow()
{
	int index = m_Grid.GetCurrentRow();
	if (index > -1)
	{
		CString temp_string;
		temp_string.Format("%s 행을 삭제하시겠습니까?", m_Grid.QuickGetText(0, index));
		if (AfxMessageBox(temp_string, MB_YESNO) == IDYES)
		{
			auto index_itr = find(m_lstGridIndex.begin(), m_lstGridIndex.end(), index);
			int data_index = index_itr - m_lstGridIndex.begin();

			for (int i = data_index + 1; i < m_lstGridIndex.size(); i++)
			{
				if (m_lstGridIndex[i] > -1)
				{
					m_lstGridIndex[i] = m_lstGridIndex[i] - 1;
					temp_string.Format("%d", atoi(m_lstGridText[i][0]) - 1);
					m_lstGridText[i][0] = temp_string;
				}
			}
			m_lstGridIndex.erase(index_itr);
			auto data_itr = m_lstGridText.begin() + data_index;
			m_lstGridText.erase(data_itr);

			m_Grid.DeleteRow(index);

			int row_count = m_Grid.GetNumberRows();
			for (int i = index; i < row_count; i++)
			{
				int new_index = atoi(m_Grid.QuickGetText(0, i)) - 1;
				temp_string.Format("%d", new_index);
				m_Grid.QuickSetText(0, i, temp_string);
			}

			m_Grid.RedrawAll();
		}
	}
}


void CPDFIndexExplorerView::OnOpenSingle()
{
	CFileDialog dlg(TRUE, "xlsx", "", 6UL, "Excel(*.xlsx)|*.xlsx|All Files(*.*)|*.*||");
	if (dlg.DoModal() == IDOK)
	{
		m_Grid.SetPaintMode(FALSE);
		char strUtf8[512] = { 0, };
		int row = 1;
		int col_size = 1;
		CString no;

		lpctstr_to_utf8(dlg.GetPathName(), strUtf8);
		std::string s(strUtf8);
		xlnt::workbook wb;
		wb.load(s);

		int sheet_count = wb.sheet_count();
		if (sheet_count > 0)
		{
			xlnt::worksheet ws = wb.sheet_by_index(0);
			while (ws.cell(col_size, row).has_value())
			{
				m_Grid.SetNumberCols(col_size + 1);
				m_Grid.QuickSetText(col_size, row - 2, utf_to_multibyte(ws.cell(col_size, row).to_string()).c_str());
				m_Grid.QuickSetCellType(col_size, -1, 0);
				col_size++;
			}
			m_Grid.QuickSetText(0, row - 2, "No");
			col_size++;

			//m_lstFilter.resize(col_size);

			row++;
			while (ws.cell(1, row).has_value())
			{
				m_Grid.SetNumberRows(row - 1);
				m_lstGridText.resize(row - 1);
				m_lstGridText[row - 2].resize(col_size-2);
				m_lstGridIndex.resize(row - 1);

				no.Format("%d", row - 1);

				m_Grid.QuickSetText(0, row - 2, no);
				//m_lstGridText[row - 2][0] = no;
				m_lstGridIndex[row - 2] = row - 2;

				for (int i = 1; i < col_size-1; i++)
				{
					CString cell_string(utf_to_multibyte(ws.cell(i, row).to_string()).c_str());
					m_Grid.QuickSetText(i, row - 2, cell_string);
					m_lstGridText[row - 2][i-1] = cell_string;
					//m_lstFilter[i][cell_string] = true;
				}
				//CString cell_string(utf_to_multibyte(ws.cell(26, row).to_string()).c_str());	//	z열(상대경로)
				////m_Grid.QuickSetText(col_size - 1, row - 2, cell_string);
				//m_lstGridText[row - 2][col_size - 1] = cell_string;
				row++;
			}
		}

		m_Grid.SetPaintMode(TRUE);
	}
}


//void CPDFIndexExplorerView::OnOpenMerge()
//{
//	m_iGridRowCount = m_lstGridText.size();
//	int row_count = m_Grid.GetNumberRows();
//	int col_count = m_Grid.GetNumberCols();
//
//	if (m_iGridRowCount < 1)
//	{
//		OnOpenSingle();
//	}
//	else
//	{
//		CString ext = m_strCurrentSelectedItemPath.Right(m_strCurrentSelectedItemPath.GetLength() - m_strCurrentSelectedItemPath.ReverseFind('.') - 1);
//		CString folder_path = m_strCurrentSelectedItemPath.Left(m_strCurrentSelectedItemPath.ReverseFind('\\'));
//		CString file_name = m_strCurrentSelectedItemPath.Right(m_strCurrentSelectedItemPath.GetLength() - m_strCurrentSelectedItemPath.ReverseFind('\\') - 1);
//
//		if (ext == "xls" || ext == "xlsx")
//		{
//			m_Grid.SetPaintMode(FALSE);
//			if (m_strCurrentFolderPath != folder_path)
//			{
//				if (AfxMessageBox("상위 폴더 경로가 달라 새로 열게 됩니다.\n\n계속 하시겠습니까 ?", MB_YESNO) == IDYES)
//				{
//					OnOpenSingle();
//				}
//			}
//			else
//			{
//				bool file_total_mode = false;
//				if (file_name.Find("통합") > -1)
//				{
//					file_total_mode = true;
//				}
//				else
//				{
//					file_total_mode = false;
//				}
//
//				if (!is_total_mode)
//				{
//					CString prev_file_name = m_strCurrentExcelPath.Right(m_strCurrentExcelPath.GetLength() - m_strCurrentExcelPath.ReverseFind('\\') - 1);
//					prev_file_name = prev_file_name.Left(prev_file_name.GetLength() - 12);
//					m_Grid.InsertCol(2);
//					m_Grid.QuickSetText(2, -1, "폴더분류");
//					for (int i = 0; i < row_count; i++)
//					{
//						m_Grid.QuickSetText(2, i, prev_file_name);
//					}
//
//					for (int i = 0; i < m_iGridRowCount; i++)
//					{
//						m_lstGridText[i].insert(m_lstGridText[i].begin() + 2, prev_file_name);
//					}
//
//					is_total_mode = true;
//					map<CString, bool> filter;
//					filter[prev_file_name] = false;
//					m_lstFilter.insert(m_lstFilter.begin() + 2, filter);
//					col_count++;
//				}
//
//				m_strCurrentExcelPath = m_strCurrentSelectedItemPath;
//				if (file_total_mode)
//				{
//					//	통합 모드에서 통합 파일 열때
//					char strUtf8[512] = { 0, };
//					int row = 1;
//					CString no;
//
//					lpctstr_to_utf8(m_strCurrentExcelPath, strUtf8);
//					std::string s(strUtf8);
//					xlnt::workbook wb;
//					wb.load(s);
//
//					int sheet_count = wb.sheet_count();
//					if (sheet_count > 0)
//					{
//						xlnt::worksheet ws = wb.sheet_by_index(0);
//						row++;
//						while (ws.cell(1, row).has_value())
//						{
//							m_Grid.SetNumberRows(row - 1 + row_count);
//							m_lstGridText.resize(row - 1 + m_iGridRowCount);
//							m_lstGridText[row - 2 + m_iGridRowCount].resize(col_count);
//							m_lstGridIndex.resize(row - 1 + m_iGridRowCount);
//							m_lstGridIndex[row - 2 + m_iGridRowCount] = row - 2 + row_count;
//
//							no.Format("%d", row - 1 + m_iGridRowCount);
//							m_Grid.QuickSetText(0, row - 2 + row_count, no);
//							m_lstGridText[row - 2 + m_iGridRowCount][0] = no;
//
//							for (int i = 1; i < col_count - 1; i++)
//							{
//								CString cell_string(utf_to_multibyte(ws.cell(i, row).to_string()).c_str());
//								m_Grid.QuickSetText(i, row - 2 + row_count, cell_string);
//								m_lstGridText[row - 2 + m_iGridRowCount][i] = cell_string;
//								m_lstFilter[i][cell_string] = true;
//							}
//
//							CString exist_file_path = GetExistingFilePath(folder_path, m_Grid.QuickGetText(2, row - 2 + row_count), m_Grid.QuickGetText(1, row - 2 + row_count));
//							//	파일 경로
//							m_Grid.QuickSetText(col_count - 1, row - 2 + row_count, exist_file_path);
//							m_lstGridText[row - 2 + m_iGridRowCount][col_count - 1] = exist_file_path;
//							row++;
//						}
//					}
//				}
//				else
//				{
//					//	통합 모드에서 일반 파일 열때
//					char strUtf8[512] = { 0, };
//					int row = 1;
//					CString no;
//
//					lpctstr_to_utf8(m_strCurrentExcelPath, strUtf8);
//					std::string s(strUtf8);
//					xlnt::workbook wb;
//					wb.load(s);
//					file_name = m_strCurrentExcelPath.Right(m_strCurrentExcelPath.GetLength() - m_strCurrentExcelPath.ReverseFind('\\') - 1);
//					file_name = file_name.Left(file_name.GetLength() - 12);
//
//					int sheet_count = wb.sheet_count();
//					if (sheet_count > 0)
//					{
//						xlnt::worksheet ws = wb.sheet_by_index(0);
//						row++;
//						while (ws.cell(1, row).has_value())
//						{
//							m_Grid.SetNumberRows(row - 1 + row_count);
//							m_lstGridText.resize(row - 1 + m_iGridRowCount);
//							m_lstGridText[row - 2 + m_iGridRowCount].resize(col_count);
//							m_lstGridIndex.resize(row - 1 + m_iGridRowCount);
//							m_lstGridIndex[row - 2 + m_iGridRowCount] = row - 2 + row_count;
//
//							no.Format("%d", row - 1 + m_iGridRowCount);
//							m_Grid.QuickSetText(0, row - 2 + row_count, no);
//							m_lstGridText[row - 2 + m_iGridRowCount][0] = no;
//
//							CString cell_string(utf_to_multibyte(ws.cell(1, row).to_string()).c_str());
//							m_Grid.QuickSetText(1, row - 2 + row_count, cell_string);
//							m_lstGridText[row - 2 + m_iGridRowCount][1] = cell_string;
//							m_lstFilter[1][cell_string] = true;
//							
//							m_Grid.QuickSetText(2, row - 2 + row_count, file_name);
//							m_lstGridText[row - 2 + m_iGridRowCount][2] = file_name;
//							m_lstFilter[2][cell_string] = true;
//
//							for (int i = 2; i < col_count - 1; i++)
//							{
//								CString cell_string(utf_to_multibyte(ws.cell(i, row).to_string()).c_str());
//								m_Grid.QuickSetText(i + 1, row - 2 + row_count, cell_string);
//								m_lstGridText[row - 2 + m_iGridRowCount][i + 1] = cell_string;
//								m_lstFilter[i+1][cell_string] = true;
//							}
//
//							CString exist_file_path = GetExistingFilePath(folder_path, m_Grid.QuickGetText(2, row - 2 + row_count), m_Grid.QuickGetText(1, row - 2 + row_count));
//							//	파일 경로
//							m_Grid.QuickSetText(col_count - 1, row - 2 + row_count, exist_file_path);
//							m_lstGridText[row - 2 + m_iGridRowCount][col_count - 1] = exist_file_path;
//							row++;
//						}
//					}
//				}
//			}
//
//			m_Grid.SetPaintMode(TRUE);
//			FitGrid();
//		}
//	}
//}
//
//
//void CPDFIndexExplorerView::OnUpdateOpenMerge(CCmdUI *pCmdUI)
//{
//	pCmdUI->Enable(m_Grid.GetNumberRows() > 0);
//}

void CPDFIndexExplorerView::OnCheckWord()
{
	CMFCRibbonCheckBox2* pEdit = (CMFCRibbonCheckBox2*)DYNAMIC_DOWNCAST(CMFCRibbonCheckBox, m_pFrame->m_wndRibbonBar.FindByID(ID_CHECK_WORD));
	if (pEdit)
	{
		pEdit->SetCheck(!pEdit->IsChecked());
	}
}

void CPDFIndexExplorerView::OnCheckUpperlower()
{
	CMFCRibbonCheckBox2* pEdit = (CMFCRibbonCheckBox2*)DYNAMIC_DOWNCAST(CMFCRibbonCheckBox, m_pFrame->m_wndRibbonBar.FindByID(ID_CHECK_UPPERLOWER));
	if (pEdit)
	{
		pEdit->SetCheck(!pEdit->IsChecked());
	}
}

void CPDFIndexExplorerView::OnCheckBookmark()
{
	CMFCRibbonCheckBox2* pEdit = (CMFCRibbonCheckBox2*)DYNAMIC_DOWNCAST(CMFCRibbonCheckBox, m_pFrame->m_wndRibbonBar.FindByID(ID_CHECK_BOOKMARK));
	if (pEdit)
	{
		pEdit->SetCheck(!pEdit->IsChecked());
	}
}

void CPDFIndexExplorerView::OnCheckAnnotation()
{
	CMFCRibbonCheckBox2* pEdit = (CMFCRibbonCheckBox2*)DYNAMIC_DOWNCAST(CMFCRibbonCheckBox, m_pFrame->m_wndRibbonBar.FindByID(ID_CHECK_ANNOTATION));
	if (pEdit)
	{
		pEdit->SetCheck(!pEdit->IsChecked());
	}
}


void CPDFIndexExplorerView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		if (m_pFrame->GetFocus())
		{
			if (!(m_pFrame->GetFocus()->m_hWnd == m_pFrame->m_wndFileView.m_hWnd || m_pFrame->GetFocus()->m_hWnd == m_pFrame->m_wndFileView.m_wndFileView.m_hWnd))
			{
				m_pFrame->m_wndFileView.SetFocus();
			}
			else
			{
				//KillTimer(1);
			}

		}

	}
	else if (nIDEvent == 2)
	{
		CWnd* pwnd = GetFocus();

		if (pwnd)
		{
			bool is_focused = false;
			while (pwnd)
			{
				if (pwnd->m_hWnd == m_Grid.m_hWnd)
				{
					is_focused = true;
					break;
				}
				else
				{
					pwnd = pwnd->GetParent();
				}
			}
			if (!is_focused)
			{
				this->SetFocus();
			}
			else
			{
				KillTimer(2);
			}
		}
	}
	else if (nIDEvent == 3)
	{
		if (m_pFrame->GetFocus())
		{
			if (!(m_pFrame->GetFocus()->m_hWnd == m_pFrame->m_wndOutput.m_hWnd || m_pFrame->GetFocus()->m_hWnd == m_pFrame->m_wndOutput.m_wndOutputFind.m_hWnd))
			{
				m_pFrame->m_wndOutput.m_wndOutputFind.SetFocus();
			}
			else
			{
				//KillTimer(1);
			}

		}
	}
	CFormView::OnTimer(nIDEvent);
}


void CPDFIndexExplorerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	KillTimer(1);
	KillTimer(3);

	CFormView::OnLButtonDown(nFlags, point);
}


void CPDFIndexExplorerView::OnSetFocus(CWnd* pOldWnd)
{
	CFormView::OnSetFocus(pOldWnd);

	KillTimer(1);
	KillTimer(3);
}

void CPDFIndexExplorerView::OnCellChange(int oldcol, int newcol, long oldrow, long newrow)
{
	if (oldrow != newrow && newrow >= 0)
	{
		//int col_size = m_Grid.GetNumberCols();
		//CString file_path = GetFilePath(m_strCurrentFolderPath, GetPathFromGridList(newrow), m_Grid.QuickGetText(3, newrow));
		//if (IsPDF(file_path))
		//{
		//	m_pFrame->m_wndProperties.DoPreview(file_path);
		//	m_Grid.SetFocus();
		//	//SetTimer(2, 10, NULL);
		//}
	}
}


void CPDFIndexExplorerView::OnFileView()
{
	m_pFrame->m_wndFileView.ShowPane(!(m_pFrame->m_wndFileView.IsVisible()), FALSE, FALSE);
}


void CPDFIndexExplorerView::OnUpdateFileView(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pFrame->m_wndFileView.IsVisible());
}


void CPDFIndexExplorerView::OnOutputWnd()
{
	m_pFrame->m_wndOutput.ShowPane(!(m_pFrame->m_wndOutput.IsVisible()), FALSE, FALSE);
}


void CPDFIndexExplorerView::OnUpdateOutputWnd(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pFrame->m_wndOutput.IsVisible());
}


void CPDFIndexExplorerView::OnPropertiesWnd()
{
	m_pFrame->m_wndProperties.ShowPane(!(m_pFrame->m_wndProperties.IsVisible()), FALSE, FALSE);
}


void CPDFIndexExplorerView::OnUpdatePropertiesWnd(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pFrame->m_wndProperties.IsVisible());
}


void CPDFIndexExplorerView::OnSetPassword()
{
	vector< CString > file_list;
	if (m_strCurrentSelectedItemPath.Right(3).MakeLower() == "pdf")
	{
		file_list.push_back(m_strCurrentSelectedItemPath);
	}
	else
	{
		FolderGetPDFFiles(m_strCurrentSelectedItemPath, file_list);
	}

	if (file_list.size() > 0)
	{
		CString temp_string;
		temp_string.Format("Input Password for [%d] files", file_list.size());
		CInputDlg pDlg(this, "파일 암호화", temp_string);
		if (pDlg.DoModal() == IDOK)
		{
			CString password = pDlg.m_strInput;
			if (password != "")
			{
				for (int i = 0; i < file_list.size(); i++)
				{
					CString cmdLine;
					cmdLine.Format("-encrypt 40bit \"\" %s \"%s\" -o \"%s\"", password, file_list[i], file_list[i]);
					ShellExecute(m_hWnd, "open", m_strAppPath + "\\cpdf.exe", cmdLine, nullptr, SW_HIDE);
				}
			}
		}
	}
}


void CPDFIndexExplorerView::OnUnsetPassword()
{
	vector< CString > file_list;
	if (m_strCurrentSelectedItemPath.Right(3).MakeLower() == "pdf")
	{
		file_list.push_back(m_strCurrentSelectedItemPath);
	}
	else
	{
		FolderGetPDFFiles(m_strCurrentSelectedItemPath, file_list);
	}

	if (file_list.size() > 0)
	{
		CString temp_string;
		temp_string.Format("Input Password for [%d] files", file_list.size());
		CInputDlg pDlg(this, "파일 암호 해제", temp_string);
		if (pDlg.DoModal() == IDOK)
		{
			CString password = pDlg.m_strInput;
			if (password != "")
			{
				for (int i = 0; i < file_list.size(); i++)
				{
					CString cmdLine;
					cmdLine.Format("-decrypt \"%s\" owner=%s user=%s -o \"%s\"", file_list[i], password, password, file_list[i]);
					ShellExecute(m_hWnd, "open", m_strAppPath + "\\cpdf.exe", cmdLine, nullptr, SW_HIDE);
				}
			}
		}
	}
}

bool CPDFIndexExplorerView::CheckEncrypted(CString file_path)
{
	bool is_encrypted = false;
	if (file_path.Right(3).MakeLower() == "pdf")
	{
		CString strFile;
		fz_context *ctx;
		fz_document* doc;

		char strUtf8[512] = { 0, };
		lpctstr_to_utf8(file_path, strUtf8);
		std::string s(strUtf8);

		ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		fz_try(ctx) {
			fz_register_document_handler(ctx, &pdf_document_handler);
			doc = fz_open_document(ctx, strUtf8);
			if (fz_needs_password(doc) == 1)
				is_encrypted = true;
		}fz_catch(ctx) {
			fz_free_context(ctx);
			return is_encrypted;
		}
		fz_close_document(doc);
		fz_free_context(ctx);
	}
	return is_encrypted;
}

void CPDFIndexExplorerView::SetEncrypted(CString file_path, CString password)
{
	CString cmdLine;
	cmdLine.Format("-encrypt 40bit \"\" %s \"%s\" -o \"%s\"", password, m_strCurrentSelectedItemPath, m_strCurrentSelectedItemPath);
	ShellExecute(m_hWnd, "open", m_strAppPath + "\\cpdf.exe", cmdLine, nullptr, SW_HIDE);
}

void CPDFIndexExplorerView::OnDClicked(int col, long row)
{
	if (row >= 0)
	{
		int col_size = m_Grid.GetNumberCols();
		CString file_path = GetFilePath(m_strCurrentFolderPath, GetPathFromGridList(row), m_Grid.QuickGetText(3, row));
		if (IsPDF(file_path))
		{
			ShellExecute(m_hWnd, "open", file_path, nullptr, nullptr, SW_SHOW);
		}
	}
}