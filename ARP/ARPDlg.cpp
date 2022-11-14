
// ARPDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ARP.h"
#include "ARPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CARPDlg 대화 상자



CARPDlg::CARPDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ARP_DIALOG, pParent)
	, CBaseLayer("ChatDlg") //
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Protocol Layer Setting
	//m_LayerMgr.AddLayer(new CChatAppLayer("ChatApp"));
	m_LayerMgr.AddLayer(new CEthernetLayer("Ethernet"));
	//m_LayerMgr.AddLayer(new FileTransLayer("FileTrans"));
	m_LayerMgr.AddLayer(new CNILayer("Network"));
	m_LayerMgr.AddLayer(this);

	// 레이어를 연결한다. (레이어 생성)
	m_LayerMgr.ConnectLayers("Network ( *Ethernet ( *ChatApp ( *ChatDlg ) *FileTrans ( *ChatDlg ) ) )");

	m_Network = (CNILayer*)m_LayerMgr.GetLayer("Network");
}

void CARPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONTROL, m_ctrlListControl);
	DDX_Control(pDX, IDC_LIST_CONTROL_PROXY, m_ctrlListControlProxy);
	DDX_Control(pDX, IDC_IPADDRESS, m_IPADDRESS);
	DDX_Control(pDX, IDC_IPADDRESS2, m_IPADDRESS2);		//<-용도가 정확치 않아 변수명을 이렇게 정했습니다.
	DDX_Control(pDX, IDC_EDIT_HW_ADDR, m_editHWAddr);
	DDX_Control(pDX, IDC_EDIT_1, m_edit1);				//<-용도가 정확치 않아 변수명을 이렇게 정했습니다.
	DDX_Control(pDX, IDC_COMBO1, m_Combox1);		//파워포인트에서 VMware 적힌 부분입니다.
	
	
}

BEGIN_MESSAGE_MAP(CARPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CARPDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CARPDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_ITEM_DEL, &CARPDlg::OnBnClickedButtonItemDel)
	ON_BN_CLICKED(IDC_BUTTON_ALL_DEL, &CARPDlg::OnBnClickedButtonAllDel)
END_MESSAGE_MAP()


// CARPDlg 메시지 처리기

BOOL CARPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	theApp.MainDlg = (CARPDlg*)AfxGetApp()->m_pMainWnd;

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	InitFn();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CARPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CARPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CARPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CARPDlg::InitFn()
{
	m_IPADDRESS.SetWindowTextW(_T("0.0.0.0"));
	m_IPADDRESS2.SetWindowTextW(_T("0.0.0.0"));
	m_editHWAddr.SetWindowTextW(_T("00:00:00:00:00:00"));
	m_edit1.SetWindowTextW(_T("00:00:00:00:00:00"));
	//--------------------------------------------------------------------------------------
	// 
	//
	//ARP Cache Set
	//
	// 
	//--------------------------------------------------------------------------------------
	m_ctrlListControl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ctrlListControl.InsertColumn(0, _T("IP Address"));
	m_ctrlListControl.SetColumnWidth(0, 120);
	m_ctrlListControl.InsertColumn(1, _T("Ethernet Address"));
	m_ctrlListControl.SetColumnWidth(1, 140);
	m_ctrlListControl.InsertColumn(2, _T("Status"));
	m_ctrlListControl.SetColumnWidth(2, 90);

	//--------------------------------------------------------------------------------------
	// 
	//
	//Proxy ARP Cache Set
	//
	// 
	//--------------------------------------------------------------------------------------
	m_ctrlListControlProxy.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ctrlListControlProxy.InsertColumn(0, _T("Device"));
	m_ctrlListControlProxy.SetColumnWidth(0, 70);
	m_ctrlListControlProxy.InsertColumn(1, _T("IP Address"));
	m_ctrlListControlProxy.SetColumnWidth(1, 120);
	m_ctrlListControlProxy.InsertColumn(2, _T("Ethernet Address"));
	m_ctrlListControlProxy.SetColumnWidth(2, 140);
	
	//--------------------------------------------------------------------------------------
	// 
	//
	//
	//
	// 
	//--------------------------------------------------------------------------------------
	m_Combox1.AddString(_T("VMware Virtual Ethernet Adapter"));
	m_Combox1.SetCurSel(0);
	
	//--------------------------------------------------------------------------------------
	// 
	//
	//Device Add Dlg Set
	//
	// 
	//--------------------------------------------------------------------------------------
	mDeviceAddDlg.Create(IDD_DIALOG_DEVICE_ADD, this);
	mDeviceAddDlg.ShowWindow(SW_HIDE);
	//
	//
	// //--------------------------------------------------------------------------------------
	// 
	//
	//테스트용으로 넣은겁니다 지우셔도됩니다.
	//
	// 
	AddArpCache(_T("192.168.011.111"), _T("00:00:00:00:00:00"), _T("InComplete"));
	AddArpCache(_T("192.168.011.222"), _T("01:00:00:00:00:00"), _T("InComplete"));
	AddArpCache(_T("192.168.011.333"), _T("02:00:00:00:00:00"), _T("InComplete"));
	//--------------------------------------------------------------------------------------
}



void CARPDlg::AddArpCache(TCHAR* _ip, TCHAR* _Ethernet, TCHAR* _Status)
{
	if (_ip == NULL) { return; }

	int nListIndex = m_ctrlListControl.GetItemCount();

	m_ctrlListControl.InsertItem(nListIndex, _ip);

	m_ctrlListControl.SetItemText(nListIndex, 1, _Ethernet);
	m_ctrlListControl.SetItemText(nListIndex, 2, _Status);
	m_ctrlListControl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlListControl.SendMessage(WM_VSCROLL, SB_BOTTOM);	// Scroll to bottom
}

void CARPDlg::AddProxyArpCache(TCHAR* _Device, TCHAR* _ip, TCHAR* _Ethernet)
{
	if (_ip == NULL) { return; }

	int nListIndex = m_ctrlListControlProxy.GetItemCount();

	m_ctrlListControlProxy.InsertItem(nListIndex, _Device);

	m_ctrlListControlProxy.SetItemText(nListIndex, 1, _ip);
	m_ctrlListControlProxy.SetItemText(nListIndex, 2, _Ethernet);
	m_ctrlListControlProxy.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlListControlProxy.SendMessage(WM_VSCROLL, SB_BOTTOM);	// Scroll to bottom
}

void CARPDlg::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	mDeviceAddDlg.InitDeviceAddDlg(0);
	mDeviceAddDlg.ShowWindow(SW_SHOW);
}


void CARPDlg::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlListControlProxy.GetFirstSelectedItemPosition();
	int nItem = m_ctrlListControlProxy.GetNextSelectedItem(pos);
	m_ctrlListControlProxy.DeleteItem(nItem);
}


void CARPDlg::OnBnClickedButtonItemDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlListControl.GetFirstSelectedItemPosition();
	int nIndex = m_ctrlListControl.GetNextSelectedItem(pos);
	if (nIndex > -1)
	{
		m_ctrlListControl.DeleteItem(nIndex);
	}
}


void CARPDlg::OnBnClickedButtonAllDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ctrlListControl.DeleteAllItems();
}
