// CRoutingTableEntry.cpp: 구현 파일
//

#include "pch.h"
#include "ARP.h"
#include "afxdialogex.h"
#include "CRoutingTableEntry.h"


// CRoutingTableEntry 대화 상자

IMPLEMENT_DYNAMIC(CRoutingTableEntry, CDialogEx)

CRoutingTableEntry::CRoutingTableEntry(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ROUTING_TABLE, pParent)
{

}

CRoutingTableEntry::~CRoutingTableEntry()
{
}

void CRoutingTableEntry::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_IPADDRESS_ROUTING_DESTINATION, m_IPADDRESS_Destination);
	DDX_Control(pDX, IDC_IPADDRESS_ROUTING_NETMASK, m_IPADDRESS_NetMask);
	DDX_Control(pDX, IDC_IPADDRESS_ROUTING_GATEWAY, m_IPADDRESS_GateWay);
	DDX_Control(pDX, IDC_COMBO_ROUTING_INTERFACE, m_RoutingInterface);

	DDX_Control(pDX, IDC_CHECK_ROUTING_UP, m_RoutingCheckUp);
	DDX_Control(pDX, IDC_CHECK_ROUTING_DOWN, m_RoutingCheckDown);
	DDX_Control(pDX, IDC_CHECK_ROUTING_HOST, m_RoutingCheckHost);


	//IDC_CHECK_ROUTING_UP
	//IDC_CHECK_ROUTING_DOWN
	//IDC_CHECK_ROUTING_HOST
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRoutingTableEntry, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRoutingTableEntry::OnBnClickedOk)
END_MESSAGE_MAP()


// CRoutingTableEntry 메시지 처리기


BOOL CRoutingTableEntry::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_RoutingInterface.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CRoutingTableEntry::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString mDestination;
	CString mNetmask;
	CString mGateway;
	CString mFlag;
	UCHAR ip[IP_ADDR_SIZE] = { 0, };
	CString mInterface;
	m_IPADDRESS_Destination.GetAddress(ip[0], ip[1], ip[2], ip[3]);
	addrToStr(ARP_IP_TYPE, mDestination, ip);

	m_IPADDRESS_NetMask.GetAddress(ip[0], ip[1], ip[2], ip[3]);
	addrToStr(ARP_IP_TYPE, mNetmask, ip);

	m_IPADDRESS_GateWay.GetAddress(ip[0], ip[1], ip[2], ip[3]);
	addrToStr(ARP_IP_TYPE, mGateway, ip);
	
	mFlag.Empty();
	if (m_RoutingCheckUp.GetCheck())
	{

		mFlag.AppendFormat(_T("U"));
	}
	if (m_RoutingCheckDown.GetCheck())
	{

		mFlag.AppendFormat(_T("D"));
	}
	if (m_RoutingCheckHost.GetCheck())
	{
		mFlag.AppendFormat(_T("H"));

	}
	m_RoutingInterface.GetLBText(m_RoutingInterface.GetCurSel(), mInterface);
	theApp.MainDlg->AddRoutingTable(0, mDestination, mNetmask, mGateway, mFlag, mInterface);
	
	CDialogEx::OnOK();
}



