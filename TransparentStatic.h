#pragma once


// CTransparentStatic

class CTransparentStatic : public CStatic
{
	DECLARE_DYNAMIC(CTransparentStatic)

public:
	CTransparentStatic();
	virtual ~CTransparentStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


