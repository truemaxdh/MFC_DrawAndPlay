// PicView.cpp : implementation file
//

#include "stdafx.h"
#include "Draw.h"
#include "DrawDoc.h"
#include "DrawView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawView

IMPLEMENT_DYNCREATE(CDrawView, CView)

CDrawView::CDrawView()
{
	m_colorPen = RGB(0,0,255);
	m_colorPenX = RGB(255,0,255);
	m_colorBrush = RGB(0,255,255);

	//ÃÊ±â ±×¸®±â ¸ðµå´Â ÀÚÀ¯°î¼±À¸·Î
	m_nDrawType = ID_FREEDRAW;
}

CDrawView::~CDrawView()
{
}


BEGIN_MESSAGE_MAP(CDrawView, CView)
	//{{AFX_MSG_MAP(CDrawView)
	ON_COMMAND(ID_PENCOLOR, OnPenColor)
	ON_COMMAND(ID_BRUSHCOLOR, OnBrushColor)
	ON_COMMAND(IDC_ERASER, OnEraser)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND(ID_BUTTON_PLAY, OnButtonPlay)
	ON_COMMAND(ID_BUTTON_STOP, OnButtonStop)
	ON_COMMAND(ID_BUTTON_TEMPSTOP, OnButtonTempstop)
	ON_COMMAND(ID_ADDTOLIB, OnAddtolib)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_LINE, ID_FREEDRAW, OnChangeTool)
	ON_UPDATE_COMMAND_UI_RANGE(ID_LINE, ID_FREEDRAW, OnUpdateChangeTool)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawView drawing

void CDrawView::OnInitialUpdate()
{
	
	CView::OnInitialUpdate();
	
	m_boolPause = FALSE;
	SetTimer(0,40,NULL);
	pos = NULL;
	oldPos = NULL;
	CClientDC dc(this);

	MemDC_DRAW.CreateCompatibleDC(&dc);
	MemDC_BITMAP.CreateCompatibleDC(&dc);
	bitmap.LoadBitmap(IDB_PAPERPLANE);
	DRAWBitmap.CreateCompatibleBitmap(&dc,2000,2000);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawView diagnostics

#ifdef _DEBUG
void CDrawView::AssertValid() const
{
	CView::AssertValid();
}

void CDrawView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDrawDoc* CDrawView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawDoc)));
	return (CDrawDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrawView message handlers

void CDrawView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//±×¸²À» ±×¸± Á¡µéÀ» ÃÊ±âÈ­
	m_ptOld = m_ptTrace = point;
	
	//¸¶¿ì½º°¡ ´­¸° »óÅÂ¿¡¼­ À©µµ¿ì¸¦ ºüÁ®³ª°¡´Â °æ¿ì¸¦ ´ëºñ
	SetCapture();
	
	CView::OnLButtonDown(nFlags, point);
}

void CDrawView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//OnLButtonDown¿¡¼­ Ä¸ÃÄÇÑ ¸¶¿ì½º¸¦ ³õ¾Æ ÁÜ
	ReleaseCapture();

	//ÀÚÀ¯°î¼± ±×¸®±â°¡ ¾Æ´Ñ °æ¿ì¿¡ µ¥ÀÌÅÍ¸¦ ÀúÀåÇÔ
	//ÀÚÀ¯°î¼±ÀÇ °æ¿ì¿¡´Â OnMouseMove¿¡¼­ ÀúÀå
	if( m_nDrawType != ID_FREEDRAW ) {
		CData *pNode;
		pNode = new CData;
		
		pNode->m_ptTail = point;
		pNode->m_ptHead = m_ptOld;
		pNode->m_nDrawType = m_nDrawType;
		pNode->m_colorPen = m_colorPen;
		pNode->m_colorBrush = m_colorBrush;

		GetDocument()->m_List.AddTail(pNode);
		
		CClientDC dc(this);
		DrawNode(&dc, pNode);
	}

	CView::OnLButtonUp(nFlags, point);
}

void CDrawView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( nFlags & MK_LBUTTON  ) {	//if LButton pressed
		CClientDC dc(this);

		//Ææ°ú ºê·¯½Ã ¼Ó¼ºÀ» º¯°æ
		CPen pen,*pOldPen;

		if(m_nDrawType == ID_FREEDRAW) 
		{
			pen.CreatePen(PS_SOLID,1,m_colorPen);
		}	
		else
		{
			pen.CreatePen(PS_SOLID,1,m_colorPenX);
			dc.SelectObject(GetStockObject(NULL_BRUSH));
			dc.SetROP2(R2_XORPEN);
		}
		pOldPen = (CPen *)dc.SelectObject(&pen);
	
		switch( m_nDrawType ) {
			//¼± ±×¸®±â
			case ID_LINE : 
				// ÀÌÀü ±×¸² Áö¿ì±â
				dc.MoveTo(m_ptOld.x,m_ptOld.y);
				dc.LineTo(m_ptTrace.x,m_ptTrace.y);
				// »õ ±×¸² ±×¸®±â
				dc.MoveTo(m_ptOld.x,m_ptOld.y);
				dc.LineTo(point.x,point.y);
				break;

			//»ç°¢Çü ±×¸®±â
			case ID_RECTANGLE : 					
				// ÀÌÀü ±×¸² Áö¿ì±â
				dc.Rectangle(m_ptOld.x,m_ptOld.y,m_ptTrace.x,m_ptTrace.y);
				// »õ ±×¸² ±×¸®±â	
				dc.Rectangle(m_ptOld.x,m_ptOld.y,point.x,point.y);
				break;
							
			//Å¸¿ø ±×¸®±â
			case ID_ELLIPSE : 
				// ÀÌÀü ±×¸² Áö¿ì±â
				dc.Ellipse(m_ptOld.x,m_ptOld.y,m_ptTrace.x,m_ptTrace.y);
				// »õ ±×¸² ±×¸®±â		
				dc.Ellipse(m_ptOld.x,m_ptOld.y,point.x,point.y);
			break;
				
			//ÀÚÀ¯°î¼± ±×¸®±â
			case ID_FREEDRAW :
				dc.MoveTo(m_ptOld.x,m_ptOld.y);
				dc.LineTo(point.x,point.y);
				m_ptOld = point;

				//ÀÚÀ¯°î¼±ÀÇ °æ¿ì´Â ¸¶¿ì½º¸¦ ¿òÁ÷ÀÏ ¶§¸¶´Ù Á¡À» ÀúÀå
				CData *pNode;
				pNode = new CData;
				pNode->m_ptTail = point;
				pNode->m_ptHead = m_ptTrace;
				pNode->m_nDrawType = m_nDrawType;
				pNode->m_colorPen = m_colorPen;
				pNode->m_colorBrush = m_colorBrush;

				GetDocument()->m_List.AddTail(pNode);
				break;
		}
	
		//¿òÁ÷ÀÌ´Â Á¡À» ÃßÀû
		m_ptTrace = point;

		dc.SelectObject(pOldPen);
	}
	
	CView::OnMouseMove(nFlags, point);
}

//±×¸®±â ¸ðµå º¯°æ /////////////////////////////////
void CDrawView::OnChangeTool(UINT nID) 
{
	m_nDrawType = nID;	
}


//±×¸®±â ¸ðµå¿¡ ´ëÇÑ »ç¿ëÀÚ ÀÎÅÍÆäÀÌ½º Ã³¸® ///////////
void CDrawView::OnUpdateChangeTool(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nDrawType == (int)pCmdUI->m_nID);
	
}


//ÀúÀåµÈ µ¥ÀÌÅÍ ´Ù½Ã ±×¸®±â
void CDrawView::DrawNode(CDC * pDC, CData * pNode)
{
	// ³ëµå¿¡ ÀúÀåµÇ¾î ÀÖ´Â Ææ »öÀ» ¼³Á¤
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,pNode->m_colorPen);
	pen.CreatePen(PS_SOLID,1,m_colorPen);
	pOldPen = (CPen *)MemDC_DRAW.SelectObject(&pen);

	// ³ëµå¿¡ ÀúÀåµÇ¾î ÀÖ´Â ºê·¯½Ã »öÀ» ¼³Á¤
	CBitmap *pOldDrawBitmap = (CBitmap *)MemDC_DRAW.SelectObject(&DRAWBitmap);
	CBitmap *pOldBitmap = (CBitmap *)MemDC_BITMAP.SelectObject(&bitmap);

	CBrush brush,*pOldBrush;
	brush.CreateSolidBrush(pNode->m_colorBrush);
	brush.CreateSolidBrush(m_colorBrush);
	pOldBrush = (CBrush *)MemDC_DRAW.SelectObject(&brush);
		
	// ³ëµå¿¡ ÀúÀåµÇ¾î ÀÖ´Â ±×¸²ÀÇ Å¸ÀÔ¿¡ µû¶ó °æ¿ì¸¦ ³ª´®
	switch( pNode->m_nDrawType ) {
		case ID_RECTANGLE: 	// »ç°¢Çü
			MemDC_DRAW.Rectangle( pNode->m_ptHead.x,pNode->m_ptHead.y, pNode->m_ptTail.x,pNode->m_ptTail.y);
			break; 

		case ID_ELLIPSE: 		// ¿ø
			MemDC_DRAW.Ellipse( pNode->m_ptHead.x,pNode->m_ptHead.y, pNode->m_ptTail.x,pNode->m_ptTail.y);
			break; 

		case ID_LINE: 			// Á÷¼±ÀÌ³ª ÀÚÀ¯°î¼±
		case ID_FREEDRAW: 
			MemDC_DRAW.MoveTo( pNode->m_ptHead.x,pNode->m_ptHead.y);
			MemDC_DRAW.LineTo( pNode->m_ptTail.x,pNode->m_ptTail.y);
			break; 
	}
	
	
	pDC->BitBlt(0,0,2000,2000, &MemDC_DRAW,0,0,SRCCOPY);
	pDC->BitBlt(pNode->m_ptTail.x,pNode->m_ptTail.y,55,40, &MemDC_BITMAP,0,0,SRCCOPY);
		
	MemDC_DRAW.SelectObject(pOldDrawBitmap);
	(CBitmap *)MemDC_BITMAP.SelectObject(pOldBitmap);
}

//¼± »ö ¼³Á¤
void CDrawView::OnPenColor() 
{
	CColorDialog dlg;	

	//ÄÃ·¯ ´ÙÀÌ¾ó·Î±× ¹Ú½º¸¦ ÀÌ¿ëÇØ¼­ ÄÃ·¯¸¦ ¼³Á¤. XOR ÆæÀ¸·Î ±×¸± °ÍÀÌ¹Ç·Î »öÀ» ¹Ý´ë·Î ¼³Á¤
	if( dlg.DoModal() == IDOK ) {
		m_colorPen= dlg.GetColor();
		m_colorPenX = ( RGB ( GetRValue(m_colorPen) ^ 255, GetGValue(m_colorPen) ^ 255, GetBValue(m_colorPen) ^ 255 ) ); 
	}
}

//Ã¤¿ì±â »ö ¼³Á¤
void CDrawView::OnBrushColor() 
{
	CColorDialog dlg;	
	
	//ÄÃ·¯ ´ÙÀÌ¾ó·Î±× ¹Ú½º¸¦ ÀÌ¿ëÇØ¼­ ÄÃ·¯¸¦ ¼³Á¤. XOR ÆæÀ¸·Î ±×¸± °ÍÀÌ¹Ç·Î »öÀ» ¹Ý´ë·Î ¼³Á¤
	if( dlg.DoModal() == IDOK ) {
		m_colorBrush = dlg.GetColor();
	}
}

//ÀüÃ¼ µ¥ÀÌÅÍ Áö¿ì±â
void CDrawView::OnEraser() 
{
	CDrawDoc* pDoc = GetDocument();

	//ÀüÃ¼ µ¥ÀÌÅÍ Áö¿ì±â
	pDoc->DeleteContents();

	//È­¸é ´Ù½Ã ±×¸®±â
	pDoc->UpdateAllViews(NULL);
}

void CDrawView::OnTimer(UINT nIDEvent) 
{
	// TODO: ‚±‚ÌˆÊ’u‚ÉƒƒbƒZ[ƒW ƒnƒ“ƒhƒ‰—p‚ÌƒR[ƒh‚ð’Ç‰Á‚·‚é‚©‚Ü‚½‚ÍƒfƒtƒHƒ‹ƒg‚Ìˆ—‚ðŒÄ‚Ño‚µ‚Ä‚­‚¾‚³‚¢
	if ((!m_boolPause) && (pos != NULL)) {
		CClientDC dc(this);
		CDrawDoc *pDoc = GetDocument();
		DrawNode(&dc, (CData *)pDoc->m_List.GetNext(pos));
		oldPos=pos;
	}
	CView::OnTimer(nIDEvent);
}

void CDrawView::OnDestroy() 
{
	CView::OnDestroy();
	
	// TODO: ‚±‚ÌˆÊ’u‚ÉƒƒbƒZ[ƒW ƒnƒ“ƒhƒ‰—p‚ÌƒR[ƒh‚ð’Ç‰Á‚µ‚Ä‚­‚¾‚³‚¢
	KillTimer(0);
}

void CDrawView::OnButtonPlay() 
{
	// TODO: ‚±‚ÌˆÊ’u‚ÉƒRƒ}ƒ“ƒh ƒnƒ“ƒhƒ‰—p‚ÌƒR[ƒh‚ð’Ç‰Á‚µ‚Ä‚­‚¾‚³‚¢
	m_boolPause=FALSE;
	m_boolInitiate=FALSE;
	CDrawDoc *pDoc = GetDocument();
	pDoc->UpdateAllViews(NULL);
	pos = pDoc->m_List.GetHeadPosition();

}


void CDrawView::OnButtonStop() 
{
	// TODO: ‚±‚ÌˆÊ’u‚ÉƒRƒ}ƒ“ƒh ƒnƒ“ƒhƒ‰—p‚ÌƒR[ƒh‚ð’Ç‰Á‚µ‚Ä‚­‚¾‚³‚¢
	pos = NULL;

}


void CDrawView::OnButtonTempstop() 
{
	// TODO: ‚±‚ÌˆÊ’u‚ÉƒRƒ}ƒ“ƒh ƒnƒ“ƒhƒ‰—p‚ÌƒR[ƒh‚ð’Ç‰Á‚µ‚Ä‚­‚¾‚³‚¢
	if(m_boolPause) {
		m_boolPause = FALSE;
		pos = oldPos;
	} else {
		pos = NULL;
		m_boolPause = TRUE;
	}

}



void CDrawView::OnAddtolib() 
{
	// TODO: ‚±‚ÌˆÊ’u‚ÉƒRƒ}ƒ“ƒh ƒnƒ“ƒhƒ‰—p‚ÌƒR[ƒh‚ð’Ç‰Á‚µ‚Ä‚­‚¾‚³‚¢
	
}

void CDrawView::OnDraw(CDC* pDC) 
{
	// TODO: ‚±‚ÌˆÊ’u‚ÉŒÅ—L‚Ìˆ—‚ð’Ç‰Á‚·‚é‚©A‚Ü‚½‚ÍŠî–{ƒNƒ‰ƒX‚ðŒÄ‚Ño‚µ‚Ä‚­‚¾‚³‚¢
	
	
}
