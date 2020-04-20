// DrawDoc.h : interface of the CDrawDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWDOC_H__EF46A30D_C7A0_11D1_8EB2_006097283F10__INCLUDED_)
#define AFX_DRAWDOC_H__EF46A30D_C7A0_11D1_8EB2_006097283F10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Data.h"

class CDrawDoc : public CDocument
{
protected: // create from serialization only
	CDrawDoc();
	DECLARE_DYNCREATE(CDrawDoc)

// Attributes
public:
	CObList	m_List;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWDOC_H__EF46A30D_C7A0_11D1_8EB2_006097283F10__INCLUDED_)
