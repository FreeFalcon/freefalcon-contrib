// LODConvertDoc.h : interface of the CLODConvertDoc class
//


#pragma once

class CLODConvertDoc : public CDocument
{
protected: // create from serialization only
	CLODConvertDoc();
	DECLARE_DYNCREATE(CLODConvertDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CLODConvertDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


