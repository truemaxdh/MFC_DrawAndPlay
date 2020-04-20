#if !defined(AFX_DATA_H__5AFD3A3D_8CB1_11D1_9BBC_00A0246AD770__INCLUDED_)
#define AFX_DATA_H__5AFD3A3D_8CB1_11D1_9BBC_00A0246AD770__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Data.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CData command target

class CData : public CObject
{
// Attributes
public:

// Operations
public:
	CData();
	virtual ~CData();

// Overrides
public:
	CPoint m_ptTail;
	CPoint m_ptHead;
	int m_nDrawType;
	COLORREF m_colorBrush;
	COLORREF m_colorPen;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CData)
	virtual void Serialize(CArchive &ar);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CData)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATA_H__5AFD3A3D_8CB1_11D1_9BBC_00A0246AD770__INCLUDED_)
