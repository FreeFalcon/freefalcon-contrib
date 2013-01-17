// LODConvertView.h : interface of the CLODConvertView class
//


#pragma once


class CLODConvertView : public CView
{
protected: // create from serialization only
	CLODConvertView();
	DECLARE_DYNCREATE(CLODConvertView)

// Attributes
public:
	CLODConvertDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CLODConvertView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LODConvertView.cpp
inline CLODConvertDoc* CLODConvertView::GetDocument() const
   { return reinterpret_cast<CLODConvertDoc*>(m_pDocument); }
#endif

extern	CLODConvertView	*m_DisplayArea;
