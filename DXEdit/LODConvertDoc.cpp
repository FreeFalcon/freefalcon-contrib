// LODConvertDoc.cpp : implementation of the CLODConvertDoc class
//

#include "stdafx.h"
#include "LODConvert.h"

#include "LODConvertDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLODConvertDoc

IMPLEMENT_DYNCREATE(CLODConvertDoc, CDocument)

BEGIN_MESSAGE_MAP(CLODConvertDoc, CDocument)
END_MESSAGE_MAP()


// CLODConvertDoc construction/destruction

CLODConvertDoc::CLODConvertDoc()
{
	// TODO: add one-time construction code here

}

CLODConvertDoc::~CLODConvertDoc()
{
}

BOOL CLODConvertDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CLODConvertDoc serialization

void CLODConvertDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CLODConvertDoc diagnostics

#ifdef _DEBUG
void CLODConvertDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLODConvertDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLODConvertDoc commands
