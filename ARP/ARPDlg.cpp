
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
	, CBaseLayer("Dlg")
	, m_ARPLayer(nullptr)
	, m_EtherLayer(nullptr)
	, m_NILayer(nullptr)
	, m_IPLayer(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_ARPLayer = new CARPLayer("ARP");
	m_IPLayer = new CIPLayer("Network");
	m_EtherLayer = new CEthernetLayer("Ethernet");
	m_NILayer = new CNILayer("NI");
	
	if (m_ARPLayer == nullptr || m_IPLayer == nullptr || m_EtherLayer == nullptr || m_NILayer == nullptr) {
		AfxMessageBox(_T("Fail : Layer Link"));
		return;
	}

	m_LayerMgr.AddLayer(m_ARPLayer);
	m_LayerMgr.AddLayer(m_IPLayer);
	m_LayerMgr.AddLayer(m_EtherLayer);
	m_LayerMgr.AddLayer(m_NILayer);
	m_LayerMgr.AddLayer(this);
	
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *Network ( *Dlg  -ARP ) *ARP ) )");
}

void CARPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ARPTABLE, m_ListARPTable);
	DDX_Control(pDX, IDC_COMBO_ADAPTER, m_ComboAdapter);
	DDX_Control(pDX, IDC_IPADDRESS_V4, m_Addr);
}

BEGIN_MESSAGE_MAP(CARPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_ADAPTER, &CARPDlg::OnCbnSelchangeComboAdapter)
	ON_BN_CLICKED(IDSELECT, &CARPDlg::OnBnClickedSelect)
	ON_BN_CLICKED(IDSEND, &CARPDlg::OnBnClickedSend)
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetTable();
	SetComboBox();

	unsigned char broadcast[6] = { 255, 255, 255, 255, 255, 255 };
	m_EtherLayer->SetDestinAddress(broadcast);
	
	
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

void CARPDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent) {
	case 1:
		updateTable();
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CARPDlg::OnCbnSelchangeComboAdapter()
{
	CString MAC, IPV4, IPV6;
	unsigned char* macaddr = m_NILayer->SetAdapter(m_ComboAdapter.GetCurSel());
	if (macaddr == nullptr) {
		MAC = DEFAULT_EDIT_TEXT;
	}
	else {
		MAC.Format(_T("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"), macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
		m_EtherLayer->SetSourceAddress(macaddr);
		m_NILayer->GetIPAddress(IPV4, IPV6);
	}
	CDialog::SetDlgItemTextW(IDC_EDIT_MAC, MAC);
	CDialog::SetDlgItemTextW(IDC_EDIT_IP, IPV4);
}


void CARPDlg::OnBnClickedSelect()
{
	CString MAC, IP;
	CDialog::GetDlgItemTextW(IDC_EDIT_MAC, MAC);
	CDialog::GetDlgItemTextW(IDC_EDIT_IP, IP);
	if (m_ComboAdapter.IsWindowEnabled()) {
		if (MAC != DEFAULT_EDIT_TEXT && IP != DEFAULT_EDIT_TEXT) {
			m_ComboAdapter.EnableWindow(FALSE);
			m_Addr.EnableWindow(TRUE);
			m_NILayer->Receiveflip();
			m_ARPLayer->setmyAddr(MAC, IP);
			CDialog::SetDlgItemTextW(IDSELECT, _T("ReSelect"));
			SetTimer(1, 1000,NULL);
			AfxBeginThread(m_NILayer->ThreadFunction_RECEIVE, m_NILayer);
		}
		else {
			AfxMessageBox(_T("Select other Adapter"));
		}
	}
	else {
		m_Addr.EnableWindow(FALSE);
		m_ComboAdapter.EnableWindow(TRUE);
		CDialog::SetDlgItemTextW(IDSELECT, _T("Select"));
		KillTimer(1);
		m_NILayer->Receiveflip();
	}
}


void CARPDlg::SetTable()
{
	CRect rt;
	m_ListARPTable.GetWindowRect(&rt);

	m_ListARPTable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListARPTable.InsertColumn(1, _T("IP Address"), LVCFMT_CENTER, int(rt.Width() * 0.35));
	m_ListARPTable.InsertColumn(2, _T("MAC Address"), LVCFMT_CENTER, int(rt.Width() * 0.4));
	m_ListARPTable.InsertColumn(3, _T("Status"), LVCFMT_CENTER, int(rt.Width() * 0.25));
}


void CARPDlg::SetComboBox()
{
	m_NILayer->SetAdapterComboBox(m_ComboAdapter);
}


void CARPDlg::OnBnClickedSend()
{
	CARPLayer::IP_HEADER payload = {0,};
	CString srcip;
	CDialog::GetDlgItemTextW(IDC_EDIT_IP, srcip);
	
	if (m_Addr.IsWindowEnabled() && !m_ComboAdapter.IsWindowEnabled()) {
		swscanf_s(srcip, _T("%3hhu.%3hhu.%3hhu.%3hhu"), &(payload.srcaddr[0]), &(payload.srcaddr[1]), &(payload.srcaddr[2]), &(payload.srcaddr[3]));
		m_Addr.GetAddress(payload.dstaddr[0], payload.dstaddr[1], payload.dstaddr[2], payload.dstaddr[3]);
		int check = 0;
		for (int i = 0; i < IP_ADDR_SIZE; i++) {
			check += payload.dstaddr[i];
		}
		if (check == 0 || check == 255 * 4) {
			AfxMessageBox(_T("Fail : Invalid Address"));
			return;
		}
		m_ARPLayer->Send((unsigned char*)&payload, 20);
	}
	else {
		AfxMessageBox(_T("Fail : Set Adapter first"));
		return;
	}
}


void CARPDlg::updateTable()
{
	m_ARPLayer->updateTable();
	std::vector<CARPLayer::ARP_NODE> table = m_ARPLayer->getTable();
	CTime cur = CTime::GetCurrentTime();
	
	for (int i = 0; i < table.size();i++) {
		CString dstip, dstmac;
		addrToStr(ARP_IP_TYPE, dstip, table[i].protocol_addr);
		addrToStr(ARP_ENET_TYPE, dstmac, table[i].hardware_addr);
		LVFINDINFO l = { LVFI_STRING, dstip };
		int fi = m_ListARPTable.FindItem(&l);
		if (fi == -1) {
			fi = m_ListARPTable.GetItemCount();
			m_ListARPTable.InsertItem(fi, dstip);
		}
		m_ListARPTable.SetItemText(fi, 1, dstmac);
		switch (table[i].status) {
		case ARP_TIME_OUT:
			m_ListARPTable.SetItemText(fi, 2, _T("Timeout"));
			break;
		case FALSE:
			m_ListARPTable.SetItemText(fi, 2, _T("incomplete"));
			break;
		case TRUE:
			m_ListARPTable.SetItemText(fi, 2, _T("Complete"));
			break;
		default:
			break;
		}
		
	}
}
