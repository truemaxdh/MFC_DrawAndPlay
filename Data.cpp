// Data.cpp : implementation file
//

#include "stdafx.h"
#include "Draw.h"
#include "Data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CData

CData::CData()
{
}

CData::~CData()
{
}



// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CData, CObject)
	//{{AFX_MSG_MAP(CData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CData member functions

void CData::Serialize(CArchive & ar)
{
	if(ar.IsStoring())
	{
		// save class data
		ar << m_ptHead;
		ar << m_ptTail;
		ar << m_nDrawType;
		ar << m_colorPen;
		ar << m_colorBrush;
	}
	else
	{
		// load class data
		ar >> m_ptHead;
		ar >> m_ptTail;
		ar >> m_nDrawType;
		ar >> m_colorPen;
		ar >> m_colorBrush;
	}
}
