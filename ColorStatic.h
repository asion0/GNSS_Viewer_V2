#pragma once


// CColorStatic

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

public:
	CColorStatic();
	virtual ~CColorStatic();

	void SetBgColor(COLORREF crColor)
	{ 
		if(m_crBgColor == crColor)
			return;
		m_crBgColor = crColor; 
		if(NULL == m_pBgBrush)
		{
			m_pBgBrush = new CBrush(crColor);
		}
		else
		{
			m_pBgBrush->DeleteObject();
			m_pBgBrush->CreateSolidBrush(crColor);
		}	
	}

	void SetTextColor(COLORREF crColor)
	{	
		if(m_crTextColor == crColor)
			return;
		m_crTextColor = crColor; 
		if(NULL == m_pTextBrush)
		{
			m_pTextBrush = new CBrush(crColor);
		}
		else
		{
			m_pTextBrush->DeleteObject();
			m_pTextBrush->CreateSolidBrush(crColor);
		}
	}
protected:
	DECLARE_MESSAGE_MAP()

	CBrush*	m_pTextBrush;
	CBrush*	m_pBgBrush;
	COLORREF m_crTextColor;
	COLORREF m_crBgColor;


public:
	afx_msg void OnPaint();
	afx_msg LRESULT OnSetText(WPARAM,LPARAM);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg BOOL OnEraseBkgnd(CDC* /*pDC*/);
};


