#if !defined(AFX_PICVIEW_H__AA575902_8D7B_11D1_9BBC_00A0246AD770__INCLUDED_)
#define AFX_PICVIEW_H__AA575902_8D7B_11D1_9BBC_00A0246AD770__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DrawView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDrawView view

class CDrawView : public CView
{
protected:
	CDrawView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDrawView)
 
// Attributes
public:
	CDrawDoc *GetDocument();

// Operations
public:
	//Member variables
	POSITION pos;
	POSITION oldPos;

	CDC MemDC_DRAW;
	CDC MemDC_BITMAP;
	CBitmap bitmap;
	CBitmap DRAWBitmap;
	COLORREF m_colorPen;	// 펜 컬러
	COLORREF m_colorPenX;	// 반전된 펜 컬러
	COLORREF m_colorBrush;	// 브러시 컬러
	CPoint m_ptOld;			// 마우스가 클릭 된 위치 저장
	CPoint m_ptTrace;		// 마우스가 움직이는 점 추적 
	int m_nDrawType;		// 그리기 도구 타입
	bool m_boolInitiate;
	bool m_boolPause;
	//Member function
	void DrawNode(CDC *pDC,CData *pNode);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawView)
	protected:
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDrawView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// Generated message map functions
	//{{AFX_MSG(CDrawView)
	afx_msg void OnPenColor();
	afx_msg void OnBrushColor();
	afx_msg void OnEraser();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnButtonPlay();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonTempstop();
	afx_msg void OnAddtolib();
	//}}AFX_MSG
	afx_msg void OnChangeTool(UINT nID);
	afx_msg void OnUpdateChangeTool(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};


#ifndef _DEBUG  // debug version in ChatView.cpp
inline CDrawDoc* CDrawView::GetDocument()
   { return (CDrawDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICVIEW_H__AA575902_8D7B_11D1_9BBC_00A0246AD770__INCLUDED_)
