#pragma once
#include "afxwin.h"
#include "Converter.h"
#include "Encoder.h"
#include "ObjectInstance.h"

// CLodInfo dialog

class CLodInfo : public CDialog
{
	DECLARE_DYNAMIC(CLodInfo)

public:
	CLodInfo(CWnd* pParent = NULL, CString Title = "", Converter *conversion=NULL, BRoot *Rt=NULL,
		BNodeType *TL=NULL, DWORD TC=NULL, DWORD Class=0, DWORD CTNumber=0, DWORD MType=0, DWORD VBClass=0);   // standard constructor
	virtual ~CLodInfo();

// Dialog Data
	enum { IDD = IDD_LODINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void	Report(void);
	Converter *cv;
	BRoot*	Root;
	BNodeType *TagList;
	DWORD		TagCount;
	DECLARE_MESSAGE_MAP()
	Encoder	*ecd;
	ObjectInstance	*TestObj;

	float	m_Rot;
	float	m_Pitch;
	float	m_Dist;
	CPoint		LastP;
	bool	Processed;



public:
	CListBox M_LODList;
	CListBox m_DXList;
	CListBox M_LodList2;
	CListBox m_DXList2;
	afx_msg void OnAppExit();
	afx_msg void OnClose();
	afx_msg void OnSetFocus(CWnd*);
};


typedef	struct	{
			char	*Label;
			DWORD Converter::* Variable;
} ListItemType;