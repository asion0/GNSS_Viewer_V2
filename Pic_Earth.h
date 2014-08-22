#pragma once

class CPic_Earth : public CStatic
{
public:
	CPic_Earth(void);
	~CPic_Earth(void);

	CButton m_gpCheck;
	CButton m_glCheck;
	CButton m_bdCheck;
	CButton m_gaCheck;

private:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();

	void InitCheck();
	bool m_first;
};
