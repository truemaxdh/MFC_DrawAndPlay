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
		// ���� ����Ʈ�� ����Ǿ� �ִ� ����� ������ ����
		ar << m_List.GetCount();
		// ���� ����Ʈ�� ����Ǿ� �ִ� ��� ��带 ���ʷ� ����
		for(POSITION pos = m_List.GetHeadPosition(); pos != NULL ; )
			m_List.GetNext(pos)->Serialize(ar);
	}
	else
	{
		int nCount;
		CData *pData;

		// ��ũ�� ����Ǿ� �ִ� ����� ������ �о��
		ar >> nCount;

		// ��ũ�� ����Ǿ� �ִ� ����� ������ŭ ������ ���鼭��
		for(int i=0 ; i<nCount ; i++)
		{
			pData = new CData;	// ���ο� ��带 �����
			pData->Serialize(ar);	// ��ũ���� �����͸� �о
			m_List.AddTail(pData);	// ���� ����Ʈ�� �߰���
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
