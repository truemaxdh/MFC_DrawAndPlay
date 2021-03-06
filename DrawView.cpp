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

	//초기 그리기 모드는 자유곡선으로
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
	//그림을 그릴 점들을 초기화
	m_ptOld = m_ptTrace = point;
	
	//마우스가 눌린 상태에서 윈도우를 빠져나가는 경우를 대비
	SetCapture();
	
	CView::OnLButtonDown(nFlags, point);
}

void CDrawView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//OnLButtonDown에서 캡쳐한 마우스를 놓아 줌
	ReleaseCapture();

	//자유곡선 그리기가 아닌 경우에 데이터를 저장함
	//자유곡선의 경우에는 OnMouseMove에서 저장
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

		//펜과 브러시 속성을 변경
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
			//선 그리기
			case ID_LINE : 
				// 이전 그림 지우기
				dc.MoveTo(m_ptOld.x,m_ptOld.y);
				dc.LineTo(m_ptTrace.x,m_ptTrace.y);
				// 새 그림 그리기
				dc.MoveTo(m_ptOld.x,m_ptOld.y);
				dc.LineTo(point.x,point.y);
				break;

			//사각형 그리기
			case ID_RECTANGLE : 					
				// 이전 그림 지우기
				dc.Rectangle(m_ptOld.x,m_ptOld.y,m_ptTrace.x,m_ptTrace.y);
				// 새 그림 그리기	
				dc.Rectangle(m_ptOld.x,m_ptOld.y,point.x,point.y);
				break;
							
			//타원 그리기
			case ID_ELLIPSE : 
				// 이전 그림 지우기
				dc.Ellipse(m_ptOld.x,m_ptOld.y,m_ptTrace.x,m_ptTrace.y);
				// 새 그림 그리기		
				dc.Ellipse(m_ptOld.x,m_ptOld.y,point.x,point.y);
			break;
				
			//자유곡선 그리기
			case ID_FREEDRAW :
				dc.MoveTo(m_ptOld.x,m_ptOld.y);
				dc.LineTo(point.x,point.y);
				m_ptOld = point;

				//자유곡선의 경우는 마우스를 움직일 때마다 점을 저장
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
	
		//움직이는 점을 추적
		m_ptTrace = point;

		dc.SelectObject(pOldPen);
	}
	
	CView::OnMouseMove(nFlags, point);
}

//그리기 모드 변경 /////////////////////////////////
void CDrawView::OnChangeTool(UINT nID) 
{
	m_nDrawType = nID;	
}


//그리기 모드에 대한 사용자 인터페이스 처리 ///////////
void CDrawView::OnUpdateChangeTool(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nDrawType == (int)pCmdUI->m_nID);
	
}


//저장된 데이터 다시 그리기
void CDrawView::DrawNode(CDC * pDC, CData * pNode)
{
	// 노드에 저장되어 있는 펜 색을 설정
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,pNode->m_colorPen);
	pen.CreatePen(PS_SOLID,1,m_colorPen);
	pOldPen = (CPen *)MemDC_DRAW.SelectObject(&pen);

	// 노드에 저장되어 있는 브러시 색을 설정
	CBitmap *pOldDrawBitmap = (CBitmap *)MemDC_DRAW.SelectObject(&DRAWBitmap);
	CBitmap *pOldBitmap = (CBitmap *)MemDC_BITMAP.SelectObject(&bitmap);

	CBrush brush,*pOldBrush;
	brush.CreateSolidBrush(pNode->m_colorBrush);
	brush.CreateSolidBrush(m_colorBrush);
	pOldBrush = (CBrush *)MemDC_DRAW.SelectObject(&brush);
		
	// 노드에 저장되어 있는 그림의 타입에 따라 경우를 나눔
	switch( pNode->m_nDrawType ) {
		case ID_RECTANGLE: 	// 사각형
			MemDC_DRAW.Rectangle( pNode->m_ptHead.x,pNode->m_ptHead.y, pNode->m_ptTail.x,pNode->m_ptTail.y);
			break; 

		case ID_ELLIPSE: 		// 원
			MemDC_DRAW.Ellipse( pNode->m_ptHead.x,pNode->m_ptHead.y, pNode->m_ptTail.x,pNode->m_ptTail.y);
			break; 

		case ID_LINE: 			// 직선이나 자유곡선
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

//선 색 설정
void CDrawView::OnPenColor() 
{
	CColorDialog dlg;	

	//컬러 다이얼로그 박스를 이용해서 컬러를 설정. XOR 펜으로 그릴 것이므로 색을 반대로 설정
	if( dlg.DoModal() == IDOK ) {
		m_colorPen= dlg.GetColor();
		m_colorPenX = ( RGB ( GetRValue(m_colorPen) ^ 255, GetGValue(m_colorPen) ^ 255, GetBValue(m_colorPen) ^ 255 ) ); 
	}
}

//채우기 색 설정
void CDrawView::OnBrushColor() 
{
	CColorDialog dlg;	
	
	//컬러 다이얼로그 박스를 이용해서 컬러를 설정. XOR 펜으로 그릴 것이므로 색을 반대로 설정
	if( dlg.DoModal() == IDOK ) {
		m_colorBrush = dlg.GetColor();
	}
}

//전체 데이터 지우기
void CDrawView::OnEraser() 
{
	CDrawDoc* pDoc = GetDocument();

	//전체 데이터 지우기
	pDoc->DeleteContents();

	//화면 다시 그리기
	pDoc->UpdateAllViews(NULL);
}

void CDrawView::OnTimer(UINT nIDEvent) 
{
	// TODO: 궞궻댧뭫궸긽긞긜�[긙 긪깛긤깋뾭궻긓�[긤귩믁돿궥귡궔귏궫궼긢긲긅깑긣궻룉뿚귩뚁귂뢯궢궲궘궬궠궋
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
	
	// TODO: 궞궻댧뭫궸긽긞긜�[긙 긪깛긤깋뾭궻긓�[긤귩믁돿궢궲궘궬궠궋
	KillTimer(0);
}

void CDrawView::OnButtonPlay() 
{
	// TODO: 궞궻댧뭫궸긓�}깛긤 긪깛긤깋뾭궻긓�[긤귩믁돿궢궲궘궬궠궋
	m_boolPause=FALSE;
	m_boolInitiate=FALSE;
	CDrawDoc *pDoc = GetDocument();
	pDoc->UpdateAllViews(NULL);
	pos = pDoc->m_List.GetHeadPosition();

}


void CDrawView::OnButtonStop() 
{
	// TODO: 궞궻댧뭫궸긓�}깛긤 긪깛긤깋뾭궻긓�[긤귩믁돿궢궲궘궬궠궋
	pos = NULL;

}


void CDrawView::OnButtonTempstop() 
{
	// TODO: 궞궻댧뭫궸긓�}깛긤 긪깛긤깋뾭궻긓�[긤귩믁돿궢궲궘궬궠궋
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
	// TODO: 궞궻댧뭫궸긓�}깛긤 긪깛긤깋뾭궻긓�[긤귩믁돿궢궲궘궬궠궋
	
}

void CDrawView::OnDraw(CDC* pDC) 
{
	// TODO: 궞궻댧뭫궸뚂뾎궻룉뿚귩믁돿궥귡궔갂귏궫궼딈�{긏깋긚귩뚁귂뢯궢궲궘궬궠궋
	
	
}
