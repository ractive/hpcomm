//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////



// MainFrame.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRAME_H__9FEEA1E5_03FA_11D3_AE2A_0000E8627120__INCLUDED_)
#define AFX_MAINFRAME_H__9FEEA1E5_03FA_11D3_AE2A_0000E8627120__INCLUDED_

#include "HPCommDoc.h"
#include "Settings.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RegBinding.h"

class CListBase;
class CHPListView;
class CProt;
class CFiler;
class CProtException;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool CheckClipboard(UINT& cb_format, BOOL& bInternal);
	bool bStartup;
	void ExceptionHandler(CProtException * pEx);
	void BumpComInfoDlg(CFiler * pFiler, int stage, bool bSuccess /* = true */);
	static void Disconnect();
	void ShowPanes();
	CFiler * pTopFiler;
	CFiler * pBottomFiler;

	bool PrepareExtDrop(HDROP hDrop, CFiler *&fromFiler);
	BOOL GetMRUName(int n, CString &sName);
	void UpdateAllViews(CFiler * pFromFiler, bool bTree);
	void SetSettingsPointers();
	CString sLastDir;
	CSettings settings;
	BOOL bFinishOnClose;
	// Defines the laguage of the help file. This is the same string as the
	// help file is called (without extension).
	// If the help file is called Italiano.hlp this string contains "Italiano"
	// It is stored in the registry and is initially empty.
	CString sHelpLanguage;

	CRegBinding rb;
	CImageList * pBigHPImageList;
	CImageList * pHPImageList;
	CImageList * pBigSystemImageList;
	CImageList * pSystemImageList;

	CProt * pKermit;
	CProt * pXModem;
	void CreateImageLists();
	void SetFilerPointers();
	CHPCommDoc * GetDocument();
	CListBase * GetLowerListView();
	virtual ~CMainFrame();
	CListBase* GetUpperListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	//CDialogBar      m_wndDlgBar;
	void OpenMRUDir(int n = 0);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCommset();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRefresh();
	afx_msg void OnAscii();
	afx_msg void OnBinary();
	afx_msg void OnUpdateAscii(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBinary(CCmdUI* pCmdUI);
	afx_msg void OnCapture();
	afx_msg void OnFileRename();
	afx_msg void OnFileNewfolder();
	afx_msg void OnFileProperties();
	afx_msg void OnCalculatorBackup();
public:
	afx_msg void OnCalculatorRestore();
protected:
	afx_msg void OnCalculatorKermit();
	afx_msg void OnCalculatorXmodem();
	afx_msg void OnCalculatorUpgradehp49g();
	afx_msg void OnViewSetup();
	afx_msg void OnClose();
	afx_msg void OnUpdateCalculatorXmodem(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCalculatorKermit(CCmdUI* pCmdUI);
	afx_msg void OnSetbookmark();
	afx_msg void OnUpdateCalculatorBackup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCalculatorRestore(CCmdUI* pCmdUI);
    afx_msg void OnHelpFinder();
    afx_msg void OnHelpLicense();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnHelplanguage();
	//}}AFX_MSG
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	afx_msg void OnMRUDir(UINT nID);
	DECLARE_MESSAGE_MAP()
private:
	void ChooseHelpFileName();
	bool IsArchiveFilename(const CString &name);
	bool bToTop;
	static CMainFrame * pseudoThis;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRAME_H__9FEEA1E5_03FA_11D3_AE2A_0000E8627120__INCLUDED_)
