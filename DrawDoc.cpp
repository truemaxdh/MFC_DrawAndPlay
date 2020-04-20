// DrawDoc.cpp : implementation of the CDrawDoc class
//

#include "stdafx.h"
#include "Draw.h"
#include "DrawDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc

IMPLEMENT_DYNCREATE(CDrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CDrawDoc, CDocument)
	//{{AFX_MSG_MAP(CDrawDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc construction/destruction

CDrawDoc::CDrawDoc()
{
	m_List.RemoveAll();

}

CDrawDoc::~CDrawDoc()
{
	DeleteContents();
}

BOOL CDrawDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	DeleteContents();

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDrawDoc serialization

void CDrawDoc::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		// 연결 리스트에 저장되어 있는 노드의 개수를 저장
		ar << m_List.GetCount();
		// 연결 리스트에 저장되어 있는 모든 노드를 차례로 저장
		for(POSITION pos = m_List.GetHeadPosition(); pos != NULL ; )
			m_List.GetNext(pos)->Serialize(ar);
	}
	else
	{
		int nCount;
		CData *pData;

		// 디스크에 저장되어 있는 노드의 개수를 읽어옴
		ar >> nCount;

		// 디스크에 저장되어 있는 노드의 개수만큼 루프를 돌면서…
		for(int i=0 ; i<nCount ; i++)
		{
			pData = new CData;	// 새로운 노드를 만들고…
			pData->Serialize(ar);	// 디스크에서 데이터를 읽어…
			m_List.AddTail(pData);	// 연결 리스트에 추가함
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc diagnostics

#ifdef _DEBUG
void CDrawDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDrawDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc commands

void CDrawDoc::DeleteContents() 
{
	for(POSITION pos = m_List.GetHeadPosition(); pos != NULL ; ) {
		delete m_List.GetNext(pos);
	}
	m_List.RemoveAll();
	
	CDocument::DeleteContents();
}
