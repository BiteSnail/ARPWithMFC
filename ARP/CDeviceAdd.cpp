// CDeviceAdd.cpp: 구현 파일
//

#include "pch.h"
#include "ARP.h"
#include "afxdialogex.h"
#include "CDeviceAdd.h"


// CDeviceAdd 대화 상자

IMPLEMENT_DYNAMIC(CDeviceAdd, CDialogEx)

CDeviceAdd::CDeviceAdd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DEVICE_ADD, pParent)
{

}

CDeviceAdd::~CDeviceAdd()
{
}

void CDeviceAdd::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_DEVICE_LIST, m_DeviceList);
	DDX_Control(pDX, IDC_EDIT_DEVICE_ETHERNET_ADDRESS, m_editDeviceEthernetAddr);
	DDX_Control(pDX, IDC_IPADDRESS_DEVICE_ADD, m_IPADDRESS_DEVICE_ADD);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDeviceAdd, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDeviceAdd::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDeviceAdd::OnBnClickedCancel)
	ON_CBN_CLOSEUP(IDC_COMBO_DEVICE_LIST, &CDeviceAdd::OnCbnCloseupComboDeviceList)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_LIST, &CDeviceAdd::OnCbnSelchangeComboDeviceList)
END_MESSAGE_MAP()


// CDeviceAdd 메시지 처리기


BOOL CDeviceAdd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetAdapterList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CDeviceAdd::InitDeviceAddDlg(int nTemp)	//nTemp는 사용은 안하지만 추가해놨습니다	
{
	SetAdapterList();
	
	if (nTemp >= 0) {
		m_DeviceList.DeleteString(nTemp);
		m_DeviceList.InsertString(nTemp, _T("Default"));
		m_DeviceList.SetCurSel(nTemp);
	}

	m_IPADDRESS_DEVICE_ADD.SetWindowTextW(_T(""));
	m_editDeviceEthernetAddr.SetWindowTextW(_T(""));
}

void CDeviceAdd::SetAdapterList()
{
	m_DeviceList.ResetContent();
	theApp.MainDlg->m_NILayer->SetAdapterComboBox(m_DeviceList);
}

void CDeviceAdd::UctoS(UCHAR* src, CString& dst)
{
	dst.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"),
		&src[0], &src[1], &src[2],
		&src[3], &src[4], &src[5]);
}

void CDeviceAdd::OnBnClickedOk()
{
	CString mMAC;
	UCHAR ip[IP_ADDR_SIZE] = { 0, };
	UCHAR mac[ENET_ADDR_SIZE] = { 0, };
	
	//IP, MAC
	m_IPADDRESS_DEVICE_ADD.GetAddress(ip[0], ip[1], ip[2], ip[3]);
	m_editDeviceEthernetAddr.GetWindowTextW(mMAC);
	StrToaddr(ARP_ENET_TYPE, mac, mMAC);

	//proxy data등록
	theApp.MainDlg->AddProxyArpCache(m_DeviceList.GetCurSel(), ip, mac);
	
	CDialogEx::OnOK();
}


void CDeviceAdd::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CDeviceAdd::OnCbnCloseupComboDeviceList()
{
	//int nIndex = m_DeviceList.GetCurSel();
	//memcpy(m_ucSrcAddr, theApp.MainDlg->m_Network->SetAdapter(nIndex), 6);

	//UctoS(m_ucSrcAddr, m_unSrcAddr);
	//m_editDeviceEthernetAddr.SetWindowTextW(m_unSrcAddr);
}


void CDeviceAdd::OnCbnSelchangeComboDeviceList()
{
	//CString IPv4, IPv6;
	//theApp.MainDlg->m_NILayer->GetIPAddress(IPv4, IPv6, m_DeviceList.GetCurSel());
	//m_IPADDRESS_DEVICE_ADD.SetWindowTextW(IPv4);
}
