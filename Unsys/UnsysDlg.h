#define _USE_MATH_DEFINES
// UnsysDlg.h : header file
//

#include <math.h>

#pragma once


// CUnsysDlg dialog
class CUnsysDlg : public CDialogEx
{
// Construction
public:
	CUnsysDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNSYS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CDC* cdc;

	// Drawing functions
	void drawBeam();

	void drawTriangle(LPPOINT vertices);

	void drawLeftSupport();
	void drawLeftSupportForce();

	void drawRightSupport();
	void drawRightSupportForce();

	void drawBeamDeflection();

	void drawLoadForce();
	void drawBendingMomentLocationIndicator();

	void drawDeflectionLocationIndicator();

	void computeEverything();

	void getMaterial();
	void getShape();

	float beamLength;
	float forcePosition;
	float chosenBendingMomentLocation;
	float forceValue;
	float RaValue;
	float RbValue;

	// Shape and material constants
	float J, E;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnBnClickedButton1();

	void redrawIllustration();
	
	CEdit b_editBox;
	CEdit F_editBox;
	CComboBox crossSect_Dropdown;
	CComboBox material_Dropdown;

	CStatic text_Ra;
	CStatic text_Rb;
	CStatic text_F;
	afx_msg void OnEnChangeEdit2();
	CSliderCtrl bendingMoment_slider;
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnNMCustomdrawSlider3(NMHDR* pNMHDR, LRESULT* pResult);
	CSliderCtrl forcePosition_slider;
	afx_msg void OnNMCustomdrawSlider4(NMHDR* pNMHDR, LRESULT* pResult);
	CStatic mg_point_text;
	CStatic bendingMomentMax_text;
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnCbnSelchangeCombo1();
	CStatic maxDeflection_text;
	CStatic deflection_text;
	CSliderCtrl deflection_slider;
	afx_msg void OnNMCustomdrawSlider5(NMHDR* pNMHDR, LRESULT* pResult);
	CButton shouldDrawDeflectionLine_checkbox;
	afx_msg void OnBnClickedCheck1();
};
