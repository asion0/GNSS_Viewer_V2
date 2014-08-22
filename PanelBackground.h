#pragma once


// CPanelBackground

class CPanelBackground : public CStatic
{
	DECLARE_DYNAMIC(CPanelBackground)

public:
	CPanelBackground();
	virtual ~CPanelBackground();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


