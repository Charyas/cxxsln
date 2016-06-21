
// iTunesDllUseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iTunesDllUse.h"
#include "iTunesDllUseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CiTunesDllUseDlg dialog



CiTunesDllUseDlg::CiTunesDllUseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CiTunesDllUseDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CiTunesDllUseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CiTunesDllUseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CiTunesDllUseDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CiTunesDllUseDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CiTunesDllUseDlg message handlers

BOOL CiTunesDllUseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CiTunesDllUseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CiTunesDllUseDlg::OnPaint()
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
HCURSOR CiTunesDllUseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "iTunesApi.h"
void CiTunesDllUseDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	iTunesApi* iTunes = new iTunesApi();
	//iTunesApi::InitApi();
	iTunes->InitApi();
	struct am_device_notification *notif;
	iTunes->AMDeviceNotificationSubscribe(iTunes->device_callback, 0, 0, NULL, &notif);

	iTunes->CFRunLoopRun();
}


void CiTunesDllUseDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here


	CString str("\\\\?\\usb#vid_05ac&pid_1227#cpid:8930_cprv:20_cpfm:03_scep:02_bdid:00_ecid:000003a6290a35cf_ibfl:1b#{b8085869-feb9-404b-8cb1-1e5c14fa8c54}");
	HANDLE handle = CreateFile(str, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (INVALID_HANDLE_VALUE == handle)
	{
		printf("aaaaaaaaaa");
	}
	DWORD count;
	DeviceIoControl(handle, 0x22000C, NULL, 0, NULL, 0, &count, NULL);

	CloseHandle(handle);
	return;
}
