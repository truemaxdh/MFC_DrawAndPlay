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

	//�ʱ� �׸��� ���� ���������
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
	//�׸��� �׸� ������ �ʱ�ȭ
	m_ptOld = m_ptTrace = point;
	
	//���콺�� ���� ���¿��� �����츦 ���������� ��츦 ���
	SetCapture();
	
	CView::OnLButtonDown(nFlags, point);
}

void CDrawView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//OnLButtonDown���� ĸ���� ���콺�� ���� ��
	ReleaseCapture();

	//����� �׸��Ⱑ �ƴ� ��쿡 �����͸� ������
	//������� ��쿡�� OnMouseMove���� ����
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

		//��� �귯�� �Ӽ��� ����
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
			//�� �׸���
			case ID_LINE : 
				// ���� �׸� �����
				dc.MoveTo(m_ptOld.x,m_ptOld.y);
				dc.LineTo(m_ptTrace.x,m_ptTrace.y);
				// �� �׸� �׸���
				dc.MoveTo(m_ptOld.x,m_ptOld.y);
				dc.LineTo(point.x,point.y);
				break;

			//�簢�� �׸���
			case ID_RECTANGLE : 					
				// ���� �׸� �����
				dc.Rectangle(m_ptOld.x,m_ptOld.y,m_ptTrace.x,m_ptTrace.y);
				// �� �׸� �׸���	
				dc.Rectangle(m_ptOld.x,m_ptOld.y,point.x,point.y);
				break;
							
			//Ÿ�� �׸���
			case ID_ELLIPSE : 
				// ���� �׸� �����
				dc.Ellipse(m_ptOld.x,m_ptOld.y,m_ptTrace.x,m_ptTrace.y);
				// �� �׸� �׸���		
				dc.Ellipse(m_ptOld.x,m_ptOld.y,point.x,point.y);
			break;
				
			//����� �׸���
			case ID_FREEDRAW :
				dc.MoveTo(m_ptOld.x,m_ptOld.y);
				dc.LineTo(point.x,point.y);
				m_ptOld = point;

				//������� ���� ���콺�� ������ ������ ���� ����
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
	
		//�����̴� ���� ����
		m_ptTrace = point;

		dc.SelectObject(pOldPen);
	}
	
	CView::OnMouseMove(nFlags, point);
}

//�׸��� ��� ���� /////////////////////////////////
void CDrawView::OnChangeTool(UINT nID) 
{
	m_nDrawType = nID;	
}


//�׸��� ��忡 ���� ����� �������̽� ó�� ///////////
void CDrawView::OnUpdateChangeTool(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nDrawType == (int)pCmdUI->m_nID);
	
}


//����� ������ �ٽ� �׸���
void CDrawView::DrawNode(CDC * pDC, CData * pNode)
{
	// ��忡 ����Ǿ� �ִ� �� ���� ����
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,pNode->m_colorPen);
	pen.CreatePen(PS_SOLID,1,m_colorPen);
	pOldPen = (CPen *)MemDC_DRAW.SelectObject(&pen);

	// ��忡 ����Ǿ� �ִ� �귯�� ���� ����
	CBitmap *pOldDrawBitmap = (CBitmap *)MemDC_DRAW.SelectObject(&DRAWBitmap);
	CBitmap *pOldBitmap = (CBitmap *)MemDC_BITMAP.SelectObject(&bitmap);

	CBrush brush,*pOldBrush;
	brush.CreateSolidBrush(pNode->m_colorBrush);
	brush.CreateSolidBrush(m_colorBrush);
	pOldBrush = (CBrush *)MemDC_DRAW.SelectObject(&brush);
		
	// ��忡 ����Ǿ� �ִ� �׸��� Ÿ�Կ� ���� ��츦 ����
	switch( pNode->m_nDrawType ) {
		case ID_RECTANGLE: 	// �簢��
			MemDC_DRAW.Rectangle( pNode->m_ptHead.x,pNode->m_ptHead.y, pNode->m_ptTail.x,pNode->m_ptTail.y);
			break; 

		case ID_ELLIPSE: 		// ��
			MemDC_DRAW.Ellipse( pNode->m_ptHead.x,pNode->m_ptHead.y, pNode->m_ptTail.x,pNode->m_ptTail.y);
			break; 

		case ID_LINE: 			// �����̳� �����
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

//�� �� ����
void CDrawView::OnPenColor() 
{
	CColorDialog dlg;	

	//�÷� ���̾�α� �ڽ��� �̿��ؼ� �÷��� ����. XOR ������ �׸� ���̹Ƿ� ���� �ݴ�� ����
	if( dlg.DoModal() == IDOK ) {
		m_colorPen= dlg.GetColor();
		m_colorPenX = ( RGB ( GetRValue(m_colorPen) ^ 255, GetGValue(m_colorPen) ^ 255, GetBValue(m_colorPen) ^ 255 ) ); 
	}
}

//ä��� �� ����
void CDrawView::OnBrushColor() 
{
	CColorDialog dlg;	
	
	//�÷� ���̾�α� �ڽ��� �̿��ؼ� �÷��� ����. XOR ������ �׸� ���̹Ƿ� ���� �ݴ�� ����
	if( dlg.DoModal() == IDOK ) {
		m_colorBrush = dlg.GetColor();
	}
}

//��ü ������ �����
void CDrawView::OnEraser() 
{
	CDrawDoc* pDoc = GetDocument();

	//��ü ������ �����
	pDoc->DeleteContents();

	//ȭ�� �ٽ� �׸���
	pDoc->UpdateAllViews(NULL);
}

void CDrawView::OnTimer(UINT nIDEvent) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
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
	
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
	KillTimer(0);
}

void CDrawView::OnButtonPlay() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	m_boolPause=FALSE;
	m_boolInitiate=FALSE;
	CDrawDoc *pDoc = GetDocument();
	pDoc->UpdateAllViews(NULL);
	pos = pDoc->m_List.GetHeadPosition();

}


void CDrawView::OnButtonStop() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	pos = NULL;

}


void CDrawView::OnButtonTempstop() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	
}

void CDrawView::OnDraw(CDC* pDC) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	
	
}
