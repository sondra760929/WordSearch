#pragma once

// CMFCRibbonCheckBox2 명령 대상

class CMFCRibbonCheckBox2 : public CMFCRibbonCheckBox
{
public:
	CMFCRibbonCheckBox2();
	virtual ~CMFCRibbonCheckBox2();

	void SetCheck(bool check)
	{
		m_bIsChecked = check;
	}
};


