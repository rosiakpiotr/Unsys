
// UnsysDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Unsys.h"
#include "UnsysDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUnsysDlg dialog



CUnsysDlg::CUnsysDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UNSYS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUnsysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, b_editBox);
	DDX_Control(pDX, IDC_EDIT3, F_editBox);
	DDX_Control(pDX, IDC_COMBO1, crossSect_Dropdown);

	DDX_Control(pDX, IDC_STATIC_Ra, text_Ra);
	DDX_Control(pDX, IDC_STATIC_Rb, text_Rb);
	DDX_Control(pDX, IDC_STATIC_F, text_F);
	DDX_Control(pDX, IDC_SLIDER3, bendingMoment_slider);
	DDX_Control(pDX, IDC_SLIDER4, forcePosition_slider);
	DDX_Control(pDX, IDC_STATIC_MG_POINT, mg_point_text);
	DDX_Control(pDX, IDC_STATIC_Mg, bendingMomentMax_text);
	DDX_Control(pDX, IDC_COMBO2, material_Dropdown);
	DDX_Control(pDX, IDC_STATIC_DEFLECTION, maxDeflection_text);
	DDX_Control(pDX, IDC_STATIC_DEFLECTION_POINT, deflection_text);
	DDX_Control(pDX, IDC_SLIDER5, deflection_slider);
	DDX_Control(pDX, IDC_CHECK1, shouldDrawDeflectionLine_checkbox);
}

BEGIN_MESSAGE_MAP(CUnsysDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON1, &CUnsysDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT2, &CUnsysDlg::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CUnsysDlg::OnEnChangeEdit3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &CUnsysDlg::OnNMCustomdrawSlider3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER4, &CUnsysDlg::OnNMCustomdrawSlider4)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CUnsysDlg::OnCbnSelchangeCombo2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CUnsysDlg::OnCbnSelchangeCombo1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER5, &CUnsysDlg::OnNMCustomdrawSlider5)
	ON_BN_CLICKED(IDC_CHECK1, &CUnsysDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CUnsysDlg message handlers

BOOL CUnsysDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	cdc = GetDC();

	beamLength = 4000;
	forceValue = 12000;
	forcePosition = beamLength / 4;
	chosenBendingMomentLocation = forcePosition;

	CString temp;

	temp.Format(_T("%.2f"), beamLength);
	b_editBox.SetWindowTextW(temp);

	temp.Format(_T("%.2f"), forceValue);
	F_editBox.SetWindowTextW(temp);
	
	crossSect_Dropdown.AddString(_T("kołowy ○"));
	crossSect_Dropdown.AddString(_T("prostokątny ▬"));
	crossSect_Dropdown.AddString(_T("teownik T"));
	crossSect_Dropdown.SetCurSel(0);

	material_Dropdown.AddString(_T("stal"));
	material_Dropdown.AddString(_T("aluminium"));
	material_Dropdown.AddString(_T("mosiądz"));
	material_Dropdown.SetCurSel(0);

	bendingMoment_slider.SetRange(0, beamLength);
	bendingMoment_slider.SetPos(beamLength / 4);
	deflection_slider.SetRange(0, beamLength);
	deflection_slider.SetPos(beamLength * 3/4.f);
	forcePosition_slider.SetRange(0, beamLength);
	forcePosition_slider.SetPos(beamLength / 2);

	// Send dummy event so we can later interpret it to make first
	// drawing of illustration.
	PostMessage(WM_HSCROLL, 0, (LPARAM)NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUnsysDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUnsysDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUnsysDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default


	// In OnInitDialog we purposely send one event that has pScollBar set to NULL
	// so we can recognize it here and call drawing methods.
	if (pScrollBar == NULL)
	{
		redrawIllustration();
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CUnsysDlg::redrawIllustration()
{
	// First draw invisble plane that will cover everything
	// drawn before.
	cdc->FillSolidRect(0, 0, 800, 210, RGB(240, 240, 240));

	computeEverything();

	drawBeam();

	drawLeftSupport();
	drawLeftSupportForce();

	drawRightSupport();
	drawRightSupportForce();
	
	drawBeamDeflection();

	drawLoadForce();
	drawBendingMomentLocationIndicator();
	drawDeflectionLocationIndicator();
}


void CUnsysDlg::drawBeam()
{
	CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
	cdc->SelectObject(pen);
	cdc->FillSolidRect(100, 100, beamLength * 0.1, 4, RGB(0, 0, 0));
}


void CUnsysDlg::drawTriangle(LPPOINT vertices)
{
	// Create outer line
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	cdc->SelectObject(pen);
	cdc->Polygon(vertices, 3);

	// Fill triangle area with color
	CRgn regionToFill;
	CBrush brush;
	regionToFill.CreatePolygonRgn(vertices, 3, ALTERNATE);
	brush.CreateSolidBrush(RGB(0, 100, 255));
	cdc->FillRgn(&regionToFill, &brush);
}


void CUnsysDlg::drawLeftSupport()
{
	// Triangle vertices
	POINT vertices[] = {
		{100, 100},
		{65, 135},				
		{135, 135}
	};
	drawTriangle(vertices);
}


void CUnsysDlg::drawLeftSupportForce()
{
	int arrowLength = 50;
	CPen redPen(PS_SOLID, 3, RGB(255, 0, 0));
	cdc->SelectObject(redPen);
	cdc->MoveTo(100, 102);
	cdc->LineTo(92, 115); 
	cdc->MoveTo(100, 102);
	cdc->LineTo(107, 115);
	cdc->MoveTo(100, 102);
	cdc->LineTo(100, 102 + arrowLength);

	text_Ra.MoveWindow(107, 100 + arrowLength / 1.3, 100, 20);
	CString temp;
	temp.Format(_T("Ra = %.2f N"), RaValue);
	text_Ra.SetWindowTextW(temp);
}


void CUnsysDlg::drawRightSupport()
{
	// Triangle vertices
	POINT vertices[] = {
		{100 + beamLength * 0.1, 100},
		{65 + beamLength * 0.1, 135},
		{135 + beamLength * 0.1, 135}
	};
	drawTriangle(vertices);

	cdc->FillSolidRect(
		65 + beamLength * 0.1, 
		138, 
		35 * 2, 
		4, 
		RGB(0, 100, 255)
	);
}


void CUnsysDlg::drawRightSupportForce()
{
	int arrowLength = 55;
	CPen redPen(PS_SOLID, 3, RGB(255, 0, 0));
	cdc->SelectObject(redPen);
	cdc->MoveTo(100 + beamLength * 0.1, 102);
	cdc->LineTo(100 + beamLength * 0.1 - 8, 115);
	cdc->MoveTo(100 + beamLength * 0.1, 102);
	cdc->LineTo(100 + beamLength * 0.1 + 8, 115);
	cdc->MoveTo(100 + beamLength * 0.1, 102);
	cdc->LineTo(100 + beamLength * 0.1, 102 + arrowLength);

	text_Rb.MoveWindow(100 + beamLength * 0.1 + 7, 100 + arrowLength / 1.25, 100, 20);
	CString temp;
	temp.Format(_T("Rb = %.2f N"), RbValue);
	text_Rb.SetWindowTextW(temp);
}


void CUnsysDlg::drawBeamDeflection()
{
	// Plot deflection using given function
	CPen pen(PS_SOLID, 1, RGB(50, 0, 50));
	cdc->SelectObject(pen);
	cdc->MoveTo(100, 100);

	// Shorter names
	float 
		F = forceValue, 
		a = forcePosition, 
		b = beamLength;

	// Integration constatns
	auto C1_f = [&](int x) -> float {
		return -(F * pow(a, 3) - 3 * F * pow(a, 2) * b + 2 * F * a * pow(b, 2)) / (6 * b);
	};

	auto C2_f = [&](int x) -> float {
		return (F * pow(a, 3) - F * a * pow(b, 2)) / (6 * b);
	};

	auto y1 = [&](int x, float C1) -> float {
		return (C1 * x - (F * pow(x, 3) * (a - b)) / (6 * b)) / (E * J);
	};

	auto y2 = [&](int x, float C2) -> float {
		return ((F * a * pow(x, 3)) / (6 * b) + C2 * x) / (E * J);
	};


	float maxDeflection = 0;
	if (shouldDrawDeflectionLine_checkbox.GetCheck())
	{
		// First section
		for (int x = 0; x < a; x += 1)
		{
			float c1 = C1_f(x);
			float val = -y1(x, c1);

			if (val > maxDeflection) maxDeflection = val;

			if ((100 + val) > 205)
			{
				cdc->MoveTo(100 + x * 0.1, 100 + val);
			}
			else {
				cdc->LineTo(100 + x * 0.1, 100 + val);
			}
		}

		cdc->MoveTo(100 + b * 0.1, 100);

		// Second section
		for (int x = 0; x < b-a; x += 2)
		{
			float c2 = C2_f(x);
			float val = -y2(x, c2);

			if (val > maxDeflection) maxDeflection = val;

			if ((100 + val) > 205)
			{
				cdc->MoveTo(100 + b * 0.1 - x * 0.1, 100 + val);
			}
			else {
				cdc->LineTo(100 + b * 0.1 - x * 0.1, 100 + val);
			}
		}
	}

	CString temp;
	temp.Format(_T("%.2f mm"), maxDeflection);
	maxDeflection_text.SetWindowTextW(temp);

	int pos = deflection_slider.GetPos();
	temp.Format(_T("%.2f mm"), pos < a ? -y1(pos, C1_f(pos)) : -y2(b - pos, C2_f(b-pos)));
	deflection_text.SetWindowTextW(temp);
}

void CUnsysDlg::drawLoadForce()
{
	int arrowLength = 60;
	CPen pen(PS_SOLID, 3, RGB(0, 255, 0));
	cdc->SelectObject(pen);
	cdc->MoveTo(100 + forcePosition * 0.1, 98);
	cdc->LineTo(100 + forcePosition * 0.1 - 8, 83);
	cdc->MoveTo(100 + forcePosition * 0.1, 98);
	cdc->LineTo(100 + forcePosition * 0.1 + 8, 83);
	cdc->MoveTo(100 + forcePosition * 0.1, 98);
	cdc->LineTo(100 + forcePosition * 0.1, 98 - arrowLength);

	text_F.MoveWindow(100 + forcePosition * 0.1 + 7, 100 - arrowLength / 1.1, 100, 20);
	CString temp;
	temp.Format(_T("F = %.2f N"), forceValue);
	text_F.SetWindowTextW(temp);
}

void CUnsysDlg::drawBendingMomentLocationIndicator()
{
	int markerWidth = 8;
	int markerHeight = 16;
	cdc->FillSolidRect(
		100 + chosenBendingMomentLocation * 0.1 - markerWidth / 2.f, 
		100 - markerHeight / 4.f, 
		markerWidth, 
		markerHeight, 
		RGB(200, 200, 100)
	);
}

void CUnsysDlg::drawDeflectionLocationIndicator()
{
	int pos = deflection_slider.GetPos();

	int markerWidth = 8;
	int markerHeight = 16;
	cdc->FillSolidRect(
		100 + pos * 0.1 - markerWidth / 2.f,
		100 - markerHeight / 4.f,
		markerWidth,
		markerHeight,
		RGB(100, 200, 200)
	);
}

void CUnsysDlg::computeEverything()
{
	RaValue = forceValue * (1 - (forcePosition / beamLength));
	RbValue = forceValue * forcePosition / beamLength;

	getMaterial();
	getShape();

	float bendingMomentInPoint, maxBendingMoment;
	// Compute bending moment in point
	if (chosenBendingMomentLocation < forcePosition)
	{
		bendingMomentInPoint = RaValue * chosenBendingMomentLocation;
	}
	else {
		bendingMomentInPoint = RbValue * (beamLength - chosenBendingMomentLocation);
	}

	maxBendingMoment = RaValue * forcePosition;
	
	CString temp;
	temp.Format(_T("%.2f Nm"), maxBendingMoment / 1000.f);
	bendingMomentMax_text.SetWindowTextW(temp);

	temp.Format(_T("%.2f Nm"), bendingMomentInPoint / 1000.f);
	mg_point_text.SetWindowTextW(temp);
}

void CUnsysDlg::getMaterial()
{
	// Set Young constant based on selection
	CString temp;
	material_Dropdown.GetWindowTextW(temp);

	if (temp == _T("stal"))
	{
		E = 2e5;
		TRACE(_T("Hi material1\n"));
	}
	else if (temp == _T("aluminium"))
	{
		E = 0.7e5;
		TRACE(_T("Hi material2\n"));
	}
	else if (temp == _T("mosiądz"))
	{
		E = 1.15e5;
		TRACE(_T("Hi material3\n"));
	}
}

void CUnsysDlg::getShape()
{
	CString temp;
	crossSect_Dropdown.GetWindowTextW(temp);

	if (temp.Find(_T("koło"), 0))
	{
		int r = 100;
		J = M_PI / 4 * pow(r, 4);
		TRACE(_T("Hi shape1\n"));
	}
	else if (temp.Find(_T("prostokąt")))
	{
		int b = 100, h = 150;
		J = b * pow(h, 3) / 12.f;
		TRACE(_T("Hi shape2\n"));
	}
	else if (temp.Find(_T("teownik")))
	{
		// Precalculated based on 
		// https://calcresource.com/moment-of-inertia-tee.html
		// for 
		// h = 150mm
		// b = 150mm
		// tf = 30mm
		// tw = 30mm
		J = 1.59075e7;
		TRACE(_T("Hi shape3\n"));
	}
}


void CUnsysDlg::OnBnClickedButton1()
{
	computeEverything();
	redrawIllustration();
}


void CUnsysDlg::OnEnChangeEdit2()
{
	CString temp;
	b_editBox.GetWindowTextW(temp);
	beamLength = _ttof(temp);

	// When beam length changes we have to update range of 
	// slider that chooses bending moment value at given point.
	bendingMoment_slider.SetRange(0, beamLength);
	deflection_slider.SetRange(0, beamLength);

	// Also update force slider
	forcePosition_slider.SetRange(0, beamLength);
}


void CUnsysDlg::OnEnChangeEdit3()
{
	CString temp;
	F_editBox.GetWindowTextW(temp);
	forceValue = _ttof(temp);
}


void CUnsysDlg::OnNMCustomdrawSlider3(NMHDR* pNMHDR, LRESULT* pResult)
{
	chosenBendingMomentLocation = bendingMoment_slider.GetPos();
	computeEverything();
	redrawIllustration();
}


void CUnsysDlg::OnNMCustomdrawSlider4(NMHDR* pNMHDR, LRESULT* pResult)
{
	forcePosition = forcePosition_slider.GetPos();
	computeEverything();
	redrawIllustration();
}


void CUnsysDlg::OnCbnSelchangeCombo2()
{
	computeEverything();
	redrawIllustration();
}


void CUnsysDlg::OnCbnSelchangeCombo1()
{
	computeEverything();
	redrawIllustration();
}


void CUnsysDlg::OnNMCustomdrawSlider5(NMHDR* pNMHDR, LRESULT* pResult)
{
	computeEverything();
	redrawIllustration();
}


void CUnsysDlg::OnBnClickedCheck1()
{
	redrawIllustration();
}
