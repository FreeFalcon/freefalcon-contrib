// LODConvertView.cpp : implementation of the CLODConvertView class
//

#include "stdafx.h"
#include "LODConvert.h"

#include "LODConvertDoc.h"
#include "LODConvertView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLODConvertView

IMPLEMENT_DYNCREATE(CLODConvertView, CView)

BEGIN_MESSAGE_MAP(CLODConvertView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CLODConvertView construction/destruction

CLODConvertView::CLODConvertView()
{
	// TODO: add construction code here

}

CLODConvertView::~CLODConvertView()
{
}

BOOL CLODConvertView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLODConvertView drawing

void CLODConvertView::OnDraw(CDC* /*pDC*/)
{
	CLODConvertDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CLODConvertView printing

BOOL CLODConvertView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLODConvertView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLODConvertView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CLODConvertView diagnostics

#ifdef _DEBUG
void CLODConvertView::AssertValid() const
{
	CView::AssertValid();
}

void CLODConvertView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLODConvertDoc* CLODConvertView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLODConvertDoc)));
	return (CLODConvertDoc*)m_pDocument;
}
#endif //_DEBUG


// CLODConvertView message handlers
